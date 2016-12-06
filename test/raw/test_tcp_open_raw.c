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

/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <uv.h>

#include "runner.h"

#if defined(__TUV_HOST_IPEXIST__)
#include "tuv_host_ipaddress.h"
#else
#pragma message("Please create tuv_host_ipaddress.h for tcp_open test.")
#endif

// Please create a file with your host IP address like below.
// This information is use to connect to host that should be running
// "tuvhelper" prgoram.
/*
#ifndef __tuv_host_ipaddress_header__
#define __tuv_host_ipaddress_header__

#define HOST_IP_ADDRESS   "192.168.1.2"

#endif
*/


#if defined(__TUV_HOST_IPEXIST__)

static int shutdown_cb_called = 0;
static int connect_cb_called = 0;
static int write_cb_called = 0;
static int close_cb_called = 0;

static uv_connect_t connect_req;
static uv_shutdown_t shutdown_req;
static uv_write_t write_req;


static void startup(void) {
  uv_sleep(1);
}


static uv_os_sock_t create_tcp_socket(void) {
  uv_os_sock_t sock;

  sock = tuvp_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  TUV_ASSERT(sock >= 0);

  {
    /* Allow reuse of the port. */
    int yes = 1;
    int r = tuvp_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    TUV_ASSERT(r == 0);
  }

  return sock;
}


static void alloc_cb(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf) {
#ifdef EMBED_LOW_MEMORY
  static char slab[2048];
#else
  static char slab[65536];
#endif
  TUV_ASSERT(suggested_size <= sizeof(slab));
  buf->base = slab;
  buf->len = sizeof(slab);
}


static void close_cb(uv_handle_t* handle) {
  TUV_ASSERT(handle != NULL);
  close_cb_called++;
}


static void shutdown_cb(uv_shutdown_t* req, int status) {
  TUV_ASSERT(req == &shutdown_req);
  TUV_ASSERT(status == 0);

  /* Now we wait for the EOF */
  shutdown_cb_called++;
}


static void read_cb(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf) {
  TUV_ASSERT(tcp != NULL);

  if (nread > 0) {
    TUV_ASSERT(nread == 4);
    TUV_ASSERT(memcmp("PING", buf->base, nread) == 0);

    /* tell server to close connection */
    {
      //TDDDLOG("read_cb, sending QS to close this");
      uv_buf_t buf = uv_buf_init((char*)"QS", 2);
      int r = uv_write(&write_req, tcp, &buf, 1, NULL);
      TUV_ASSERT(r == 0);
    }
  }
  else if (nread == 0) {

  }
  else if (nread < 0) {
    TUV_ASSERT(nread == UV_EOF);
    uv_close((uv_handle_t*)tcp, close_cb);
  }
}


static void write_cb(uv_write_t* req, int status) {
  TUV_ASSERT(req != NULL);

  if (status) {
    TDLOG("tcp_open uv_write error: %s", uv_strerror(status));
    TUV_ASSERT(0);
  }

  write_cb_called++;
}


static void connect_cb(uv_connect_t* req, int status) {
  uv_buf_t buf = uv_buf_init((char*)"PING", 4);
  uv_stream_t* stream;
  int r;

  if (status == UV__ECONNREFUSED) {
    TDLOG("tcp_open Connection refused. Run server first.");
    TUV_ASSERT(0);
  }

  printf(">> connect_cb status(%d)", status);

  TUV_ASSERT(req == &connect_req);
  TUV_ASSERT(status == 0);

  stream = req->handle;
  connect_cb_called++;

  r = uv_write(&write_req, stream, &buf, 1, write_cb);
  TUV_ASSERT(r == 0);

  /* Shutdown on drain. */
  // r = uv_shutdown(&shutdown_req, stream, shutdown_cb);
  //TUV_ASSERT(r == 0);

  /* Start reading */
  r = uv_read_start(stream, alloc_cb, read_cb);
  TUV_ASSERT(r == 0);
}


typedef struct {
  uv_loop_t* loop;
  uv_tcp_t client;
  uv_os_sock_t sock;
} tcp_open_param_t;


static int tcp_open_loop(void* vparam) {
  tcp_open_param_t* param = (tcp_open_param_t*)vparam;
  return uv_run(param->loop, UV_RUN_ONCE);
}


static int tcp_open_final(void* vparam) {
  tcp_open_param_t* param = (tcp_open_param_t*)vparam;

  TUV_ASSERT(0 == uv_loop_close(param->loop));

  //TUV_ASSERT(shutdown_cb_called == 1);
  TUV_ASSERT(connect_cb_called == 1);
  TUV_ASSERT(write_cb_called == 1);
  TUV_ASSERT(close_cb_called == 1);

  // cleanup tuv param
  free(param);

  // jump to next test
  run_tests_continue();

  return 0;
}
#endif


TEST_IMPL(tcp_open) {
#if defined(__TUV_HOST_IPEXIST__)
  tcp_open_param_t* param;
  param = (tcp_open_param_t*)malloc(sizeof(tcp_open_param_t));
  param->loop = uv_default_loop();

  struct sockaddr_in addr;
  int r;

  shutdown_cb_called = 0;
  connect_cb_called = 0;
  write_cb_called = 0;
  close_cb_called = 0;

  TUV_ASSERT(0 == uv_ip4_addr(HOST_IP_ADDRESS, TEST_PORT, &addr));

  startup();
  param->sock = create_tcp_socket();

  r = uv_tcp_init(param->loop, &param->client);
  TUV_ASSERT(r == 0);

  r = uv_tcp_open(&param->client, param->sock);
  TUV_ASSERT(r == 0);

  r = uv_tcp_connect(&connect_req,
                     &param->client,
                     (const struct sockaddr*)&addr,
                     connect_cb);
  TUV_ASSERT(r == 0);

  tuv_run(param->loop, tcp_open_loop, tcp_open_final, param);
#else
  TDDLOG("'HOST_IP_ADDRESS' not defined. skip test.\r\n");
  run_tests_continue();
#endif
  return 0;
}
