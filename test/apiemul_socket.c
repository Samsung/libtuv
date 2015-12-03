/* Copyright 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uv.h>

#include "apiemul.h"


//----------------------------------------------------------------------------

#define DUMP_HEAP_ADDRESS 0

#define SOCKET_TEST_PORT 7777
#define SOCKET_TEST_CLIENT_MAX 16
#define SOCKET_TEST_READBUFFER 1024

//----------------------------------------------------------------------------

// task for network poll
static uv_thread_t _thread;

static struct sockaddr_in _addr_in;
static int _sock_server;

static int sock_clients[SOCKET_TEST_CLIENT_MAX];
static int sock_client_cnt;

static unsigned char read_buffer[SOCKET_TEST_READBUFFER];


//----------------------------------------------------------------------------

static void test_init_clients(void) {
  int i;
  sock_client_cnt = 0;
  for (i=0;i<SOCKET_TEST_CLIENT_MAX; i++) {
    sock_clients[i] = -1;
  }
}

static int test_client_count(void) {
  return sock_client_cnt;
}

static int test_client_add(int fd) {
  int i;
  for (i=0;i<SOCKET_TEST_CLIENT_MAX; i++) {
    if (sock_clients[i] < 0) {
      sock_clients[i] = fd;
      sock_client_cnt++;
      return fd;
    }
  }
  return -1;
}

static int test_client_sub(int fd) {
  int i;
  for (i=0;i<SOCKET_TEST_CLIENT_MAX; i++) {
    if (sock_clients[i] == fd) {
      sock_clients[i] = -1;
      sock_client_cnt--;
      return fd;
    }
  }
  return -1;
}

static int test_client_fillfds(struct pollfd* fds) {
  int i, j;
  for (i=0, j=0;i<SOCKET_TEST_CLIENT_MAX; i++) {
    if (sock_clients[i] > 0) {
      fds[j].fd = sock_clients[i];
      fds[j].events = POLLIN | POLLHUP;
      j++;
    }
  }
  return j;
}

//----------------------------------------------------------------------------

static int inet_pton4(const char *src, unsigned char *dst) {
  static const char digits[] = "0123456789";
  int saw_digit, octets, ch;
  unsigned char tmp[sizeof(struct in_addr)], *tp;

  saw_digit = 0;
  octets = 0;
  *(tp = tmp) = 0;
  while ((ch = *src++) != '\0') {
    const char *pch;

    if ((pch = strchr(digits, ch)) != NULL) {
      unsigned int nw = *tp * 10 + (pch - digits);

      if (saw_digit && *tp == 0)
        return UV_EINVAL;
      if (nw > 255)
        return UV_EINVAL;
      *tp = nw;
      if (!saw_digit) {
        if (++octets > 4)
          return UV_EINVAL;
        saw_digit = 1;
      }
    } else if (ch == '.' && saw_digit) {
      if (octets == 4)
        return UV_EINVAL;
      *++tp = 0;
      saw_digit = 0;
    } else
      return UV_EINVAL;
  }
  if (octets < 4)
    return UV_EINVAL;
  memcpy(dst, tmp, sizeof(struct in_addr));
  return 0;
}

static int ip4_addr(const char* ip, int port, struct sockaddr_in* addr) {
  memset(addr, 0, sizeof(*addr));
  addr->sin_family = AF_INET;
  addr->sin_port = port;
  return inet_pton4(ip, (unsigned char *)&(addr->sin_addr.s_addr));
}

static void dump_socket_info(int sockfd) {
  struct sockaddr_in addrthis;
  socklen_t len = sizeof(addrthis);
  union {
    uint8_t addr8[4];
    uint32_t addr32;
  } c4;

  int ret = tuvp_getsockname(sockfd, (struct sockaddr*)&addrthis, &len);
  c4.addr32 = addrthis.sin_addr.s_addr;
  if (ret == 0) {
    printf("%d.%d.%d.%d:%d",
           c4.addr8[0], c4.addr8[1], c4.addr8[2], c4.addr8[3],
           addrthis.sin_port);
  }
  else {
    printf("(invalid socket)");
  }
}

static void test_new_connection(int sockserver) {
  struct sockaddr addr_client;
  int clisockfd;
  socklen_t socklen;

  memset(&addr_client, 0, sizeof(addr_client));
  socklen = (socklen_t)sizeof(addr_client);
  clisockfd = tuvp_accept(sockserver, &addr_client, &socklen);
  assert(addr_client.sa_family == AF_INET);

  printf(".. new connection to ");
  dump_socket_info(clisockfd);

  struct sockaddr_in* addrin = (struct sockaddr_in*)&addr_client;
  union {
    uint8_t addr8[4];
    uint32_t addr32;
  } c4;
  c4.addr32 = addrin->sin_addr.s_addr;
  printf(" fd(%d) from %d.%d.%d.%d:%d\r\n",
         clisockfd,
         c4.addr8[0], c4.addr8[1], c4.addr8[2], c4.addr8[3],
         addrin->sin_port);

  test_client_add(clisockfd);

  const char* msg = "Hello libtuv echo server!\r\nSend 'Q' to end.\r\n";
  tuvp_write(clisockfd, msg, strlen(msg));
}

static void test_handle_read(int sockfd) {
  ssize_t result;
  result = tuvp_read(sockfd, read_buffer, SOCKET_TEST_READBUFFER);
  if (result > 0) {
    read_buffer[result] = 0;
    if (read_buffer[0] == 'Q') {
      tuvp_close(sockfd);
      test_client_sub(sockfd);
      printf(".. RECV with 'Q', socket closed\r\n");
      return;
    }
    tuvp_write(sockfd, read_buffer, strlen((const char*)read_buffer));
  }
}

static void do_tcp_server_start(void) {
  int result;
  tuvp_tcp_init();

  ip4_addr("0.0.0.0", tuvp_htons(SOCKET_TEST_PORT), &_addr_in);
  _sock_server = tuvp_socket(AF_INET, SOCK_STREAM, 0);

  result = tuvp_bind(_sock_server,
                     (const struct sockaddr*)&_addr_in,
                     sizeof(_addr_in));
  if (result != 0) {
    printf("!! tuvp_bind failed (%d)\r\n", result);
    return;
  }
  result = tuvp_listen(_sock_server, SOMAXCONN);
  if (result != 0) {
    printf("!! tuvp_listen failed (%d)\r\n", result);
    return;
  }

  printf(".. echo server started at ");
  dump_socket_info(_sock_server);
  printf("\r\n");

  printf(".. use 'telnet' or somekind to test (manually)\r\n");
}

static void do_poll_test(void) {
  struct pollfd* fds;
  int fds_cnt;

  // prepare fds
  fds_cnt = 1 + test_client_count();
  fds = (struct pollfd*)malloc(sizeof(struct pollfd) * fds_cnt);
  memset(fds, 0, sizeof(struct pollfd) * fds_cnt);
  fds[0].fd = _sock_server;
  fds[0].events = POLLIN;
  test_client_fillfds(&fds[1]);

  // call poll
  if (tuvp_net_poll(fds, fds_cnt) > 0) {
    if (fds[0].revents & POLLIN) {
      // server has new connection
      test_new_connection(_sock_server);
    }
    else {
      int i;
      for (i=1; i<fds_cnt; i++) {
        if (fds[i].revents & POLLHUP) {
          // client hang-up
          tuvp_close(fds[i].fd);
          test_client_sub(fds[i].fd);
        }
        else if (fds[i].revents & POLLIN) {
          // client has sent something
          test_handle_read(fds[i].fd);
        }
      }
    }
  }
  free(fds);
}

static void task_entry(void* arg) {
  test_init_clients();
  do_tcp_server_start();
}

static int task_loop(void* arg) {
  do_poll_test();
  return 1; /* continue task */
}

/*
 * socket api testing with echo server
 *  - start server and wait for connections
 *  - use 'telnet (server_ip) 7777'
 *  - to close connection, send "Q"
 */
void apiemul_socket(void) {
  tuv_task_create(&_thread, task_entry, task_loop, NULL);
}
