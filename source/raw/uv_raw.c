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
#include <string.h>

#include <uv.h>


//-----------------------------------------------------------------------------

int get_errno(void) {
  return errno;
}
void set_errno(int err) {
  errno = err;
}


//-----------------------------------------------------------------------------

int uv__open_cloexec(const char* path, int flags) {
#if 0
  // TODO: fix this after file for mbed is working
  int err;
  int fd;

  fd = open(path, flags);
  if (fd == -1)
    return -errno;

  err = uv__cloexec(fd, 1);
  if (err) {
    uv__close(fd);
    return err;
  }
  return fd;
#else
  return -1;
#endif
}


/* Open a socket in non-blocking close-on-exec mode, atomically if possible. */
int uv__socket(int domain, int type, int protocol) {
  int sockfd;
  int err;

  sockfd = tuvp_socket(domain, type, protocol);
  if (sockfd == -1) {
    TDLOG("uv__socket tuvp_socket failed");
    return -errno;
  }

  err = uv__nonblock(sockfd, 1);
  if (err == 0)
    err = uv__cloexec(sockfd, 1);

  if (err) {
    TDLOG("uv__socket failed to set nonblock or cloexec");
    uv__close(sockfd);
    return err;
  }

  return sockfd;
}


int uv__accept(int sockfd) {
  int peerfd;
  int err;

  assert(sockfd >= 0);

  while (1) {
    peerfd = tuvp_accept(sockfd, NULL, NULL);
    if (peerfd == -1) {
      err = get_errno();
      if (err == EINTR)
        continue;
      return -err;
    }

    err = uv__cloexec(peerfd, 1);
    if (err == 0)
      err = uv__nonblock(peerfd, 1);

    if (err) {
      uv__close(peerfd);
      return err;
    }

    return peerfd;
  }
}


int uv_ip4_addr(const char* ip, int port, struct sockaddr_in* addr) {
  memset(addr, 0, sizeof(*addr));
  addr->sin_family = AF_INET;
  addr->sin_port = tuvp_htons(port);
  return uv_inet_pton(AF_INET, ip, &(addr->sin_addr.s_addr));
}


//-----------------------------------------------------------------------------

void uv__platform_invalidate_fd(uv_loop_t* loop, int fd) {
  int i;
  int nfd = loop->npollfds;
  for (i = 0; i < nfd; ++i) {
    struct pollfd* pfd = &loop->pollfds[i];
    if (fd == pfd->fd)
      pfd->fd = -1;
  }
}


int uv__nonblock(int fd, int set) {
  return 0;
}


int uv__close(int fd) {
  int saved_errno;
  int rc;

  if (fd < STDERR_FILENO) {
    TDLOG("uv__close fd is invalid descriptor: %d", fd);
  }
  assert(fd > -1);  /* Catch uninitialized io_watcher.fd bugs. */
  assert(fd > STDERR_FILENO);  /* Catch stdio close bugs. */

  saved_errno = get_errno();
  rc = tuvp_close(fd);
  if (rc == -1) {
    rc = -get_errno();
    if (rc == -EINTR)
      rc = -EINPROGRESS;  /* For platform/libc consistency. */
    set_errno(saved_errno);
  }

  return rc;
}


int uv__cloexec(int fd, int set) {
  //set_errno(EINVAL);
  //return -EINVAL;
  return 0;
}


ssize_t uv__recvmsg(int fd, struct msghdr* msg, int flags) {
  return -1;
}


//-----------------------------------------------------------------------------

void uv__handle_platform_init(uv_handle_t* handle) {
  handle->next_closing = NULL;
}


/* should not clear handle structure with memset as
 * *data can be set before calling init function */

void uv__idle_platform_init(uv_idle_t* handle) {
  QUEUE_INIT(&(handle->queue));
}


void uv__timer_platform_init(uv_timer_t* handle) {
}


void uv__async_platform_init(uv_async_t* handle) {
  QUEUE_INIT(&(handle->queue));
}


//-----------------------------------------------------------------------------

int getpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
  return 0;
}

ssize_t readv(int fd, const struct iovec* iiovec, int count) {
  return 0;
}


ssize_t writev(int fd, const struct iovec* iiovec, int count) {
  return 0;
}
