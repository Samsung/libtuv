/* Copyright 2016 Samsung Electronics Co., Ltd.
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

#ifndef __uv__udp_header__
#define __uv__udp_header__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>

/*
 * UDP support.
 */

enum uv_udp_flags {
  /* Disables dual stack mode. */
  UV_UDP_IPV6ONLY = 1,
  /*
   * Indicates message was truncated because read buffer was too small. The
   * remainder was discarded by the OS. Used in uv_udp_recv_cb.
   */
  UV_UDP_PARTIAL = 2,
  /*
   * Indicates if SO_REUSEADDR will be set when binding the handle.
   * This sets the SO_REUSEPORT socket flag on the BSDs and OS X. On other
   * Unix platforms, it sets the SO_REUSEADDR flag.  What that means is that
   * multiple threads or processes can bind to the same address without error
   * (provided they all set the flag) but only the last one to bind will receive
   * any traffic, in effect "stealing" the port from the previous listener.
   */
  UV_UDP_REUSEADDR = 4
};

typedef void (*uv_udp_send_cb)(uv_udp_send_t* req, int status);
typedef void (*uv_udp_recv_cb)(uv_udp_t* handle, ssize_t nread,
                               const uv_buf_t* buf,
                               const struct sockaddr* addr,
                               unsigned flags);

/* uv_udp_t is a subclass of uv_handle_t. */
struct uv_udp_s {
  UV_HANDLE_FIELDS
  /* read-only */
  /*
   * Number of bytes queued for sending. This field strictly shows how much
   * information is currently queued.
   */
  size_t send_queue_size;
  /*
   * Number of send requests currently in the queue awaiting to be processed.
   */
  size_t send_queue_count;
  UV_UDP_PRIVATE_FIELDS
};

/* uv_udp_send_t is a subclass of uv_req_t. */
struct uv_udp_send_s {
  UV_REQ_FIELDS
  uv_udp_t* handle;
  uv_udp_send_cb cb;
  UV_UDP_SEND_PRIVATE_FIELDS
};

typedef enum {
  UV_LEAVE_GROUP = 0,
  UV_JOIN_GROUP
} uv_membership;

int uv_udp_init(uv_loop_t*, uv_udp_t* handle);
int uv_udp_init_ex(uv_loop_t*, uv_udp_t* handle, unsigned int flags);
int uv_udp_open(uv_udp_t* handle, uv_os_sock_t sock);
int uv_udp_bind(uv_udp_t* handle, const struct sockaddr* addr,
                unsigned int flags);

int uv_udp_getsockname(const uv_udp_t* handle, struct sockaddr* name,
                       int* namelen);
int uv_udp_set_membership(uv_udp_t* handle, const char* multicast_addr,
                          const char* interface_addr, uv_membership membership);
int uv_udp_set_multicast_loop(uv_udp_t* handle, int on);
int uv_udp_set_multicast_ttl(uv_udp_t* handle, int ttl);
int uv_udp_set_multicast_interface(uv_udp_t* handle,
                                   const char* interface_addr);
int uv_udp_set_broadcast(uv_udp_t* handle, int on);
int uv_udp_set_ttl(uv_udp_t* handle, int ttl);

int uv_udp_send(uv_udp_send_t* req, uv_udp_t* handle, const uv_buf_t bufs[],
                unsigned int nbufs, const struct sockaddr* addr,
                uv_udp_send_cb send_cb);
int uv_udp_try_send(uv_udp_t* handle, const uv_buf_t bufs[],
                    unsigned int nbufs, const struct sockaddr* addr);
int uv_udp_recv_start(uv_udp_t* handle, uv_alloc_cb alloc_cb,
                      uv_udp_recv_cb recv_cb);
int uv_udp_recv_stop(uv_udp_t* handle);


#ifdef __cplusplus
}
#endif


#endif // __uv__udp_header__
