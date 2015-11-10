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

#ifndef __uv_linux_syscall_header__
#define __uv_linux_syscall_header__

#include <sys/socket.h>


// architectecture dependent
#if defined(__alpha__)
# define UV__O_CLOEXEC        0x200000
#elif defined(__hppa__)
# define UV__O_CLOEXEC        0x200000
#elif defined(__sparc__)
# define UV__O_CLOEXEC        0x400000
#else
# define UV__O_CLOEXEC        0x80000
#endif

#if defined(__alpha__)
# define UV__O_NONBLOCK       0x4
#elif defined(__hppa__)
# define UV__O_NONBLOCK       0x10004
#elif defined(__mips__)
# define UV__O_NONBLOCK       0x80
#elif defined(__sparc__)
# define UV__O_NONBLOCK       0x4000
#else
# define UV__O_NONBLOCK       0x800
#endif

#define UV__EFD_CLOEXEC       UV__O_CLOEXEC
#define UV__EFD_NONBLOCK      UV__O_NONBLOCK

#define UV__IN_CLOEXEC        UV__O_CLOEXEC
#define UV__IN_NONBLOCK       UV__O_NONBLOCK

#define UV__SOCK_CLOEXEC      UV__O_CLOEXEC
#define UV__SOCK_NONBLOCK     UV__O_NONBLOCK

/* epoll flags */
#define UV__EPOLL_CLOEXEC     UV__O_CLOEXEC
#define UV__EPOLL_CTL_ADD     1
#define UV__EPOLL_CTL_DEL     2
#define UV__EPOLL_CTL_MOD     3

#define UV__EPOLLIN           1
#define UV__EPOLLOUT          4
#define UV__EPOLLERR          8
#define UV__EPOLLHUP          16
#define UV__EPOLLONESHOT      0x40000000
#define UV__EPOLLET           0x80000000


#if defined(__x86_64__)
struct uv__epoll_event {
  uint32_t events;
  uint64_t data;
} __attribute__((packed));
#else
struct uv__epoll_event {
  uint32_t events;
  uint64_t data;
};
#endif

int uv__accept4(int fd, struct sockaddr* addr, socklen_t* addrlen, int flags);

int uv__eventfd(unsigned int count);
int uv__eventfd2(unsigned int count, int flags);

int uv__epoll_create(int size);
int uv__epoll_create1(int flags);
int uv__epoll_ctl(int epfd, int op, int fd, struct uv__epoll_event *ev);
int uv__epoll_wait(int epfd, struct uv__epoll_event* events,
                   int nevents, int timeout);
int uv__epoll_pwait(int epfd, struct uv__epoll_event* events,
                    int nevents, int timeout, uint64_t sigmask);

int uv__pipe2(int pipefd[2], int flags);

ssize_t uv__preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset);
ssize_t uv__pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset);

int uv__utimesat(int dirfd, const char* path, const struct timespec times[2],
                 int flags);

#endif // __uv_linux_syscall_header__
