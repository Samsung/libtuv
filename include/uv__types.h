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

#ifndef __uv_types_header__
#define __uv_types_header__

#ifndef __uv_header__
#error Please include with uv.h
#endif


//-----------------------------------------------------------------------------
// strcture types

// handle types
typedef struct uv_handle_s uv_handle_t;
typedef struct uv_loop_s uv_loop_t;
typedef struct uv_timer_s uv_timer_t;
typedef struct uv_idle_s uv_idle_t;
typedef struct uv_async_s uv_async_t;

// request types
typedef struct uv_req_s uv_req_t;
typedef struct uv_fs_s uv_fs_t;

// ext types
typedef struct uv__io_s uv__io_t;

//-----------------------------------------------------------------------------
// callback types

typedef void (*uv_timer_cb)(uv_timer_t* handle);
typedef void (*uv_idle_cb)(uv_idle_t* handle);
typedef void (*uv_close_cb)(uv_handle_t* handle);
typedef void (*uv_async_cb)(uv_async_t* handle);

typedef void (*uv_thread_cb)(void* arg);

typedef void (*uv_fs_cb)(uv_fs_t* req);


typedef void (*uv__io_cb)(struct uv_loop_s* loop, struct uv__io_s* w,
                          unsigned int events);
typedef void (*uv__async_cb)(struct uv_loop_s* loop, struct uv__async* w,
                             unsigned int nevents);
//-----------------------------------------------------------------------------

typedef struct {
  long tv_sec;
  long tv_nsec;
} uv_timespec_t;


typedef struct {
  uint64_t st_dev;
  uint64_t st_mode;
  uint64_t st_nlink;
  uint64_t st_uid;
  uint64_t st_gid;
  uint64_t st_rdev;
  uint64_t st_ino;
  uint64_t st_size;
  uint64_t st_blksize;
  uint64_t st_blocks;
  uint64_t st_flags;
  uint64_t st_gen;
  uv_timespec_t st_atim;
  uv_timespec_t st_mtim;
  uv_timespec_t st_ctim;
  uv_timespec_t st_birthtim;
} uv_stat_t;


//-----------------------------------------------------------------------------


#endif // __uv_types_header__
