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

#include "uv.h"


int uv_udp_bind(uv_udp_t* handle, const struct sockaddr* addr,
                unsigned int flags) {
  unsigned int addrlen;

  if (handle->type != UV_UDP)
    return UV_EINVAL;

  if (addr->sa_family == AF_INET) {
    addrlen = sizeof(struct sockaddr_in);
  } else if (addr->sa_family == AF_INET6) {
    addrlen = sizeof(struct sockaddr_in6);
  } else {
    return UV_EINVAL;
  }

  return uv__udp_bind(handle, addr, addrlen, flags);
}


int uv_udp_send(uv_udp_send_t* req, uv_udp_t* handle, const uv_buf_t bufs[],
                unsigned int nbufs, const struct sockaddr* addr,
                uv_udp_send_cb send_cb) {
  unsigned int addrlen;

  if (handle->type != UV_UDP)
    return UV_EINVAL;

  if (addr->sa_family == AF_INET) {
    addrlen = sizeof(struct sockaddr_in);
  } else if (addr->sa_family == AF_INET6) {
    addrlen = sizeof(struct sockaddr_in6);
  } else {
    return UV_EINVAL;
  }

  return uv__udp_send(req, handle, bufs, nbufs, addr, addrlen, send_cb);
}


int uv_udp_try_send(uv_udp_t* handle, const uv_buf_t bufs[],
                    unsigned int nbufs, const struct sockaddr* addr) {
  unsigned int addrlen;

  if (handle->type != UV_UDP)
    return UV_EINVAL;

  if (addr->sa_family == AF_INET) {
    addrlen = sizeof(struct sockaddr_in);
  } else if (addr->sa_family == AF_INET6) {
    addrlen = sizeof(struct sockaddr_in6);
  } else {
    return UV_EINVAL;
  }

  return uv__udp_try_send(handle, bufs, nbufs, addr, addrlen);
}


int uv_udp_recv_start(uv_udp_t* handle, uv_alloc_cb alloc_cb,
                      uv_udp_recv_cb recv_cb) {
  if (handle->type != UV_UDP || alloc_cb == NULL || recv_cb == NULL) {
    return UV_EINVAL;
  } else {
    return uv__udp_recv_start(handle, alloc_cb, recv_cb);
  }
}


int uv_udp_recv_stop(uv_udp_t* handle) {
  if (handle->type != UV_UDP) {
    return UV_EINVAL;
  } else {
    return uv__udp_recv_stop(handle);
  }
}
