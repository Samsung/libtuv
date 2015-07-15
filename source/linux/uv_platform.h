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

#ifndef __uv__platform_header__
#define __uv__platform_header__

#include <semaphore.h>
#include <pthread.h>

#include "uv_linux_syscall.h"


#ifndef TUV_POLL_EVENTS_SIZE
#define TUV_POLL_EVENTS_SIZE 1024
#endif


//-----------------------------------------------------------------------------

#define UV__POLLIN   UV__EPOLLIN
#define UV__POLLOUT  UV__EPOLLOUT
#define UV__POLLERR  UV__EPOLLERR
#define UV__POLLHUP  UV__EPOLLHUP

//-----------------------------------------------------------------------------

#define ACCESS_ONCE(type, var)                                                \
  (*(volatile type*) &(var))


#define SAVE_ERRNO(block)                                                     \
  do {                                                                        \
    int _saved_errno = errno;                                                 \
    do { block; } while (0);                                                  \
    errno = _saved_errno;                                                     \
  }                                                                           \
  while (0)


//-----------------------------------------------------------------------------
// linux loop flags

enum {
  UV_LOOP_BLOCK_SIGPROF = 1
};

//-----------------------------------------------------------------------------
// linux date time extension

typedef enum {
  UV_CLOCK_PRECISE = 0,  /* Use the highest resolution clock available. */
  UV_CLOCK_FAST = 1      /* Use the fastest clock with <= 1ms granularity. */
} uv_clocktype_t;

// in linux/uv_clock.cpp
uint64_t uv__hrtime(uv_clocktype_t type);

#define uv__update_time(loop)                                                 \
  loop->time = uv__hrtime(UV_CLOCK_FAST) / 1000000


//-----------------------------------------------------------------------------
// linux fs

typedef struct dirent uv__dirent_t;


typedef struct uv_buf_t {
  char* base;
  size_t len;
} uv_buf_t;


typedef int uv_file;


#ifdef UV__O_NONBLOCK
# define UV__F_NONBLOCK UV__O_NONBLOCK
#else
# define UV__F_NONBLOCK 1
#endif


//-----------------------------------------------------------------------------
// linux thread and mutex

#define UV_ONCE_INIT PTHREAD_ONCE_INIT

typedef pthread_t uv_thread_t;
typedef pthread_once_t uv_once_t;
typedef pthread_mutex_t uv_mutex_t;
typedef sem_t uv_sem_t;
typedef pthread_cond_t uv_cond_t;
typedef pthread_rwlock_t uv_rwlock_t;


//-----------------------------------------------------------------------------
// linux

int uv__nonblock(int fd, int set);
int uv__close(int fd);


//-----------------------------------------------------------------------------
// loop

int uv__platform_loop_init(uv_loop_t* loop);
void uv__platform_loop_delete(uv_loop_t* loop);
void uv__platform_invalidate_fd(uv_loop_t* loop, int fd);


//-----------------------------------------------------------------------------
// for async

int uv__make_pipe(int fds[2], int flags);

int uv__open_cloexec(const char* path, int flags);
int uv__cloexec(int fd, int set);

void uv__async_close(uv_async_t* handle);
int uv__async_make_pending(int* pending);


void uv__io_init(uv__io_t* w, uv__io_cb cb, int fd);
void uv__io_start(uv_loop_t* loop, uv__io_t* w, unsigned int events);
void uv__io_stop(uv_loop_t* loop, uv__io_t* w, unsigned int events);
void uv__io_close(uv_loop_t* loop, uv__io_t* w);

void uv__io_feed(uv_loop_t* loop, uv__io_t* w);
int uv__io_active(const uv__io_t* w, unsigned int events);
void uv__io_poll(uv_loop_t* loop, int timeout); /* in milliseconds or -1 */


#endif // __uv__platform_header__
