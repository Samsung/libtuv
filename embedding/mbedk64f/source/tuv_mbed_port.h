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

#ifndef __MBED_PORT_HEADER__
#define __MBED_PORT_HEADER__

// socket and file descriptor buffer size
#define TUV_MAX_FF_COUNT    16    // just an offset
#define TUV_MAX_FD_COUNT    32    // number of file descriptors to provide
#define TUV_MAX_SD_COUNT    32    // number of socket descriptors to provide

#define STDIN_FILNO         0
#define STDOUT_FILNO        1
#define STDERR_FILENO       2

#define POLLIN        0x01
#define POLLOUT       0x02
#define POLLERR       0x04
#define POLLHUP       0x08

#define PTHREAD_ONCE_INIT    0x12348765


//----------------------------------------------------------------------------

/* from LWIP, lwip/sockets.h */
#define SOL_SOCKET      0xfff     /* options for socket level */

/* Options for level IPPROTO_TCP */
#define TCP_NODELAY     0x01
#define TCP_KEEPALIVE   0x02
#define TCP_KEEPIDLE    0x03
#define TCP_KEEPINTVL   0x04
#define TCP_KEEPCNT     0x05

/* from sal/socket_types.h, shoudld sync with it */
#define AF_UNSPEC       0
#define AF_INET4        1
#define AF_INET6        2
#define AF_INET         AF_INET4
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#define IPPROTO_UDPLITE 136

#ifndef socklen_t
#  define socklen_t uint32_t
#endif

/* Socket protocol types (TCP/UDP/RAW) */
#ifndef SOCK_STREAM
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#endif

#ifndef SO_REUSEADDR
#define  SO_REUSEADDR   0x0004    /* Allow local address reuse */
#define  SO_KEEPALIVE   0x0008    /* keep connections alive */
#endif

#define SO_ERROR        0x1007    /* get error status and clear */


/* socket shutdown how */
#ifndef SHUT_RD
#define SHUT_RD         0
#define SHUT_WR         1
#define SHUT_RDWR       2
#endif


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

/* For compatibility with BSD code */
struct in_addr {
  uint32_t s_addr;
};

/* members are in network byte order */
struct sockaddr_in {
  uint8_t sin_len;
  uint8_t sin_family;
  uint16_t sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct sockaddr {
  uint8_t sa_len;
  uint8_t sa_family;
  char    sa_data[14];
};


//-----------------------------------------------------------------------------

struct pollfd {
  int   fd;         /* file descriptor */
  short events;     /* requested events */
  short revents;    /* returned events */
};


struct iovec
{
  void* iov_base;
  size_t iov_len;
};


//-----------------------------------------------------------------------------

typedef int uv_os_sock_t;


//-----------------------------------------------------------------------------
// platform port

void exit(int status);

// platform itself
void tuvp_platform_init();

// timer
void tuvp_timer_init(void);
void tuvp_timer_start(void);
void tuvp_timer_restart(void);
void tuvp_timer_stop(void);
int tuvp_timer_usec(void);

// pipe
int tuvp_pipe(int fds[2]);

// thread emulation
typedef void* tuvp_thread_t;
typedef uint32_t tuvp_thread_attr_t;
typedef uint32_t tuvp_once_t;
typedef uint32_t tuvp_mutex_t;
typedef uint32_t tuvp_sem_t;
typedef uint32_t tuvp_cond_t;
typedef uint32_t tuvp_rwlock_t;


// task, for thread emulation with mbed scheduler
typedef void (*tuv_taskentry_cb)(void* arg);
typedef int (*tuv_taskloop_cb)(void* arg);

int tuvp_task_create(tuvp_thread_t *thread,
                     tuv_taskentry_cb entry, tuv_taskloop_cb loop, void *arg);
int tuvp_task_is_running(tuvp_thread_t thread);
int tuvp_task_close(tuvp_thread_t thread);


// socket

// Maximum queue length specifiable by listen.
#define SOMAXCONN 8

void tuvp_tcp_init(void);
int tuvp_socket(int domain, int type, int protocol);
int tuvp_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int tuvp_setsockopt(int sockfd, int level, int optname, const void *optval,
                    socklen_t optlen);
int tuvp_getsockopt(int sockfd, int level, int optname, void *optval,
                    socklen_t *optlen);
int tuvp_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int tuvp_listen(int sockfd, int backlog);
int tuvp_connect(int sockfd, const struct sockaddr*addr, socklen_t addrlen);

uint16_t tuvp_htons(uint16_t hostshort);
int tuvp_net_poll(struct pollfd* fds, int nfds);
int tuvp_poll(struct pollfd *fds, int nfds, int timeout);
int tuvp_close(int sockfd);


int tuvp_shutdown(int sockfd, int how);

int tuvp_getsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
int tuvp_getpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen);

ssize_t tuvp_write(int fd, const void* buf, size_t count);
ssize_t tuvp_read(int sockfd, void *buf, size_t count);

ssize_t tuvp_readv(int fd, const struct iovec* iiovec, int count);
ssize_t tuvp_writev(int fd, const struct iovec* iiovec, int count);


#ifdef __cplusplus
}
#endif

#include <uv.h>


#endif // __MBED_PORT_HEADER__
