/* Copyright 2015-2016 Samsung Electronics Co., Ltd.
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

#ifndef UV_NUTTX_H
#define UV_NUTTX_H

#include <pthread.h>
#include <poll.h>     // nuttx poll
#include <unistd.h>
#include <errno.h>

#include <netdb.h>
#include <sys/uio.h>

#ifndef TUV_POLL_EVENTS_SIZE
#define TUV_POLL_EVENTS_SIZE  32
#endif

// TUV_CHANGES@20161130: FIXME: What is the reasonable nubmer?
#ifndef IOV_MAX
#define IOV_MAX TUV_POLL_EVENTS_SIZE
#endif

//---------------------------------------------------------------------------
// TUV_CHANGES@20161130: NuttX does not provide ENOTSUP.
// TUV_CHANGES@20170407: NuttX started to provide ENOTSUP.
#ifndef ENOTSUP
#define ENOTSUP       EOPNOTSUPP
#endif

// TUV_CHANGES@20171130:
// Not defined macros. Copied from x86-64 linux system header
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define SIGPROF       27
#define TCP_NODELAY   1

#ifndef _SC_CLK_TCK
#define _SC_CLK_TCK 0x0006
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

//-----------------------------------------------------------------------------
// date time extension
// uint64_t uv__time_precise();


//-----------------------------------------------------------------------------
// thread and mutex
#define UV_PLATFORM_RWLOCK_T pthread_mutex_t

//-----------------------------------------------------------------------------
// uio
ssize_t readv(int __fd, const struct iovec* __iovec, int __count);
ssize_t writev(int __fd, const struct iovec* __iovec, int __count);


//-----------------------------------------------------------------------------
// etc
int getpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen);

// Maximum queue length specifiable by listen
#ifndef SOMAXCONN
#define SOMAXCONN 8
#endif

//-----------------------------------------------------------------------------
// structure extension for nuttx
//
#define UV_PLATFORM_LOOP_FIELDS                                               \
  struct pollfd pollfds[TUV_POLL_EVENTS_SIZE];                                \
  int npollfds;                                                               \



#endif // UV_NUTTX_H
