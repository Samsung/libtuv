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


//-----------------------------------------------------------------------------
// loop

int uv__platform_loop_init(uv_loop_t* loop);
void uv__platform_loop_delete(uv_loop_t* loop);

//-----------------------------------------------------------------------------
// linux date time extension

typedef enum {
  UV_CLOCK_PRECISE = 0,  /* Use the highest resolution clock available. */
  UV_CLOCK_FAST = 1      /* Use the fastest clock with <= 1ms granularity. */
} uv_clocktype_t;

// in linux/uv_clock.cpp
uint64_t uv__hrtime(uv_clocktype_t type);

#define uv__update_time(loop)                                                 \
  loop->time = uv__hrtime(UV_CLOCK_FAST) / 1000000;

//-----------------------------------------------------------------------------
// linux thread and mutex

#define UV_ONCE_INIT PTHREAD_ONCE_INIT

typedef pthread_t uv_thread_t;
typedef pthread_once_t uv_once_t;
typedef pthread_mutex_t uv_mutex_t;
typedef pthread_cond_t uv_cond_t;

#endif // __uv__platform_header__
