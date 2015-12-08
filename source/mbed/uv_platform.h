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

#ifndef __uv__platform_mbed_header__
#define __uv__platform_mbed_header__


#define TUV_DECLARE_MBED_DEFS
#include "tuv_mbed_port.h"

#define TUV_POLL_EVENTS_SIZE    (TUV_MAX_FD_COUNT+TUV_MAX_SD_COUNT)

#define TUV_MBED_IOV_MAX        TUV_POLL_EVENTS_SIZE /* check this */

//-----------------------------------------------------------------------------

#define UV__POLLIN    POLLIN    /* 0x01 */
#define UV__POLLOUT   POLLOUT   /* 0x02 */
#define UV__POLLERR   POLLERR   /* 0x04 */
#define UV__POLLHUP   POLLHUP   /* 0x08 */


//-----------------------------------------------------------------------------

int get_errno(void);
void set_errno(int err);


#define SAVE_ERRNO(block)                                                     \
  do {                                                                        \
    int _saved_errno = get_errno();                                           \
    do { block; } while (0);                                                  \
    set_errno(_saved_errno);                                                  \
  }                                                                           \
  while (0)


//-----------------------------------------------------------------------------
// mbed date time extension

typedef enum {
  UV_CLOCK_PRECISE = 0,  /* Use the highest resolution clock available. */
  UV_CLOCK_FAST = 1      /* Use the fastest clock with <= 1ms granularity. */
} uv_clocktype_t;

// in mbed/uv_clock.cpp
uint64_t uv__hrtime(uv_clocktype_t type);

#define uv__update_time(loop)                                                 \
  loop->time = uv__hrtime(UV_CLOCK_FAST) / 1000000

uint64_t uv__time_precise();

void tuv__time_init(void);

//-----------------------------------------------------------------------------
// mbed thread and mutex emulation

#define UV_ONCE_INIT PTHREAD_ONCE_INIT

typedef tuvp_thread_t uv_thread_t;
typedef tuvp_once_t uv_once_t;
typedef tuvp_mutex_t uv_mutex_t;
typedef tuvp_sem_t uv_sem_t;
typedef tuvp_cond_t uv_cond_t;
typedef tuvp_rwlock_t uv_rwlock_t;

int tuv_cond_wait(uv_cond_t* cond, uv_mutex_t* mutex);


int tuv_task_create(uv_thread_t* tid, tuv_taskentry_cb entry,
                    tuv_taskloop_cb loop, void* arg);
int tuv_task_running(uv_thread_t *tid);
int tuv_task_close(uv_thread_t *tid);


//-----------------------------------------------------------------------------

void tuv__platform_init(void);

#endif // __uv__platform_mbed_header__
