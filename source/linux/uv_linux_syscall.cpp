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

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <unistd.h>
#include <sys/syscall.h>

#include <uv.h>


// linux_syscall is only for linux


//-----------------------------------------------------------------------------

#if defined(__arm__)
# if defined(__thumb__) || defined(__ARM_EABI__)
#  define UV_SYSCALL_BASE 0
# else
#  define UV_SYSCALL_BASE 0x900000
# endif
#endif /* __arm__ */


//-----------------------------------------------------------------------------

#ifndef __NR_accept4
# if defined(__x86_64__)
#  define __NR_accept4 288
# elif defined(__i386__)
   /* Nothing. Handled through socketcall(). */
# elif defined(__arm__)
#  define __NR_accept4 (UV_SYSCALL_BASE + 366)
# endif
#endif /* __NR_accept4 */


int uv__accept4(int fd, struct sockaddr* addr, socklen_t* addrlen, int flags) {
#if defined(__i386__)
  unsigned long args[4];
  int r;

  args[0] = (unsigned long) fd;
  args[1] = (unsigned long) addr;
  args[2] = (unsigned long) addrlen;
  args[3] = (unsigned long) flags;

  r = syscall(__NR_socketcall, 18 /* SYS_ACCEPT4 */, args);

  /* socketcall() raises EINVAL when SYS_ACCEPT4 is not supported but so does
   * a bad flags argument. Try to distinguish between the two cases.
   */
  if (r == -1)
    if (errno == EINVAL)
      if ((flags & ~(UV__SOCK_CLOEXEC|UV__SOCK_NONBLOCK)) == 0)
        errno = ENOSYS;

  return r;
#elif defined(__NR_accept4)
  return syscall(__NR_accept4, fd, addr, addrlen, flags);
#else
  return errno = ENOSYS, -1;
#endif
}
//-----------------------------------------------------------------------------

#ifndef __NR_eventfd
# if defined(__x86_64__)
#  define __NR_eventfd 284
# elif defined(__i386__)
#  define __NR_eventfd 323
# elif defined(__arm__)
#  define __NR_eventfd (UV_SYSCALL_BASE + 351)
# endif
#endif /* __NR_eventfd */

#ifndef __NR_eventfd2
# if defined(__x86_64__)
#  define __NR_eventfd2 290
# elif defined(__i386__)
#  define __NR_eventfd2 328
# elif defined(__arm__)
#  define __NR_eventfd2 (UV_SYSCALL_BASE + 356)
# endif
#endif /* __NR_eventfd2 */

//-----------------------------------------------------------------------------

int uv__eventfd(unsigned int count) {
#if defined(__NR_eventfd)
  return syscall(__NR_eventfd, count);
#else
  return errno = ENOSYS, -1;
#endif
}


int uv__eventfd2(unsigned int count, int flags) {
#if defined(__NR_eventfd2)
  return syscall(__NR_eventfd2, count, flags);
#else
  return errno = ENOSYS, -1;
#endif
}


//-----------------------------------------------------------------------------

#ifndef __NR_epoll_create
# if defined(__x86_64__)
#  define __NR_epoll_create 213
# elif defined(__i386__)
#  define __NR_epoll_create 254
# elif defined(__arm__)
#  define __NR_epoll_create (UV_SYSCALL_BASE + 250)
# endif
#endif /* __NR_epoll_create */

#ifndef __NR_epoll_create1
# if defined(__x86_64__)
#  define __NR_epoll_create1 291
# elif defined(__i386__)
#  define __NR_epoll_create1 329
# elif defined(__arm__)
#  define __NR_epoll_create1 (UV_SYSCALL_BASE + 357)
# endif
#endif /* __NR_epoll_create1 */

#ifndef __NR_pipe2
# if defined(__x86_64__)
#  define __NR_pipe2 293
# elif defined(__i386__)
#  define __NR_pipe2 331
# elif defined(__arm__)
#  define __NR_pipe2 (UV_SYSCALL_BASE + 359)
# endif
#endif /* __NR_pipe2 */

#ifndef __NR_epoll_ctl
# if defined(__x86_64__)
#  define __NR_epoll_ctl 233 /* used to be 214 */
# elif defined(__i386__)
#  define __NR_epoll_ctl 255
# elif defined(__arm__)
#  define __NR_epoll_ctl (UV_SYSCALL_BASE + 251)
# endif
#endif /* __NR_epoll_ctl */

#ifndef __NR_epoll_wait
# if defined(__x86_64__)
#  define __NR_epoll_wait 232 /* used to be 215 */
# elif defined(__i386__)
#  define __NR_epoll_wait 256
# elif defined(__arm__)
#  define __NR_epoll_wait (UV_SYSCALL_BASE + 252)
# endif
#endif /* __NR_epoll_wait */

#ifndef __NR_utimensat
# if defined(__x86_64__)
#  define __NR_utimensat 280
# elif defined(__i386__)
#  define __NR_utimensat 320
# elif defined(__arm__)
#  define __NR_utimensat (UV_SYSCALL_BASE + 348)
# endif
#endif /* __NR_utimensat */

#ifndef __NR_epoll_pwait
# if defined(__x86_64__)
#  define __NR_epoll_pwait 281
# elif defined(__i386__)
#  define __NR_epoll_pwait 319
# elif defined(__arm__)
#  define __NR_epoll_pwait (UV_SYSCALL_BASE + 346)
# endif
#endif /* __NR_epoll_pwait */


//-----------------------------------------------------------------------------

int uv__epoll_create(int size) {
#if defined(__NR_epoll_create)
  return syscall(__NR_epoll_create, size);
#else
  return errno = ENOSYS, -1;
#endif
}


int uv__epoll_create1(int flags) {
#if defined(__NR_epoll_create1)
  return syscall(__NR_epoll_create1, flags);
#else
  return errno = ENOSYS, -1;
#endif
}


int uv__epoll_ctl(int epfd, int op, int fd, struct uv__epoll_event* events) {
#if defined(__NR_epoll_ctl)
  return syscall(__NR_epoll_ctl, epfd, op, fd, events);
#else
  return errno = ENOSYS, -1;
#endif
}

int uv__epoll_wait(int epfd, struct uv__epoll_event* events,
                   int nevents, int timeout) {
#if defined(__NR_epoll_wait)
  return syscall(__NR_epoll_wait, epfd, events, nevents, timeout);
#else
  return errno = ENOSYS, -1;
#endif
}


int uv__epoll_pwait(int epfd, struct uv__epoll_event* events,
                    int nevents, int timeout, uint64_t sigmask) {
#if defined(__NR_epoll_pwait)
  return syscall(__NR_epoll_pwait,
                 epfd,
                 events,
                 nevents,
                 timeout,
                 &sigmask,
                 sizeof(sigmask));
#else
  return errno = ENOSYS, -1;
#endif
}


int uv__utimesat(int dirfd,
                 const char* path,
                 const struct timespec times[2],
                 int flags)
{
#if defined(__NR_utimensat)
  return syscall(__NR_utimensat, dirfd, path, times, flags);
#else
  return errno = ENOSYS, -1;
#endif
}


int uv__pipe2(int pipefd[2], int flags) {
#if defined(__NR_pipe2)
  return syscall(__NR_pipe2, pipefd, flags);
#else
  return errno = ENOSYS, -1;
#endif
}
