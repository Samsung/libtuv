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

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/ioctl.h> // FIOCLEX

#include <uv.h>



void uv__platform_invalidate_fd(uv_loop_t* loop, int fd) {
  struct uv__epoll_event* events;
  struct uv__epoll_event dummy;
  uintptr_t i;
  uintptr_t nfds;

  assert(loop->watchers != NULL);

  events = (struct uv__epoll_event*) loop->watchers[loop->nwatchers];
  nfds = (uintptr_t) loop->watchers[loop->nwatchers + 1];
  if (events != NULL)
    /* Invalidate events with same file descriptor */
    for (i = 0; i < nfds; i++)
      if ((int) events[i].data == fd)
        events[i].data = -1;

  /* Remove the file descriptor from the epoll.
   * This avoids a problem where the same file description remains open
   * in another process, causing repeated junk epoll events.
   *
   * We pass in a dummy epoll_event, to work around a bug in old kernels.
   */
  if (loop->backend_fd >= 0)
    uv__epoll_ctl(loop->backend_fd, UV__EPOLL_CTL_DEL, fd, &dummy);
}




ssize_t uv__preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset) {
#if defined(__NR_preadv)
  return syscall(__NR_preadv, fd, iov, iovcnt, offset);
#else
  return errno = ENOSYS, -1;
#endif
}


ssize_t uv__pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset) {
#if defined(__NR_pwritev)
  return syscall(__NR_pwritev, fd, iov, iovcnt, offset);
#else
  return errno = ENOSYS, -1;
#endif
}


int uv__close(int fd) {
  int saved_errno;
  int rc;

  assert(fd > -1);  /* Catch uninitialized io_watcher.fd bugs. */
  assert(fd > STDERR_FILENO);  /* Catch stdio close bugs. */

  saved_errno = errno;
  rc = close(fd);
  if (rc == -1) {
    rc = -errno;
    if (rc == -EINTR)
      rc = -EINPROGRESS;  /* For platform/libc consistency. */
    errno = saved_errno;
  }

  return rc;
}


int uv__nonblock(int fd, int set) {
  int r;

  do
    r = ioctl(fd, FIONBIO, &set);
  while (r == -1 && errno == EINTR);

  if (r)
    return -errno;

  return 0;
}


int uv__cloexec(int fd, int set) {
  int r;

  do
    r = ioctl(fd, set ? FIOCLEX : FIONCLEX);
  while (r == -1 && errno == EINTR);

  if (r)
    return -errno;

  return 0;
}


void uv__handle_platform_init(uv_handle_t* handle) {
 handle->next_closing = NULL;
}


void uv__idle_platform_init(uv_idle_t* handle) {
  memset(handle, 0, sizeof(uv_idle_t));
  QUEUE_INIT(&(handle->queue));
}


void uv__timer_platform_init(uv_timer_t* handle) {
  memset(handle, 0, sizeof(uv_timer_t));
}


void uv__async_platform_init(uv_async_t* handle) {
  memset(handle, 0, sizeof(uv_async_t));
  QUEUE_INIT(&(handle->queue));
}
