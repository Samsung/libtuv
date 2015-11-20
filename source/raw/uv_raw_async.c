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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <uv.h>


int uv__async_make_pending(int* pending) {
  /* Do a cheap read first. */
  if (ACCESS_ONCE(int, *pending) != 0)
    return 1;

#if defined(__GNUC__) && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ > 0)
  return __sync_val_compare_and_swap(pending, 0, 1) != 0;
#else
  ACCESS_ONCE(int, *pending) = 1;
  return 0;
#endif
}


//-----------------------------------------------------------------------------
//

void uv__async_init(struct uv__async* wa) {
  wa->wfd = -1;
}


void uv__async_close(uv_async_t* handle) {
  QUEUE_REMOVE(&handle->queue);
  uv__handle_stop(handle);
}


void uv__async_stop(uv_loop_t* loop, struct uv__async* wa) {
  wa->wfd = -1;
}


int uv__async_start(uv_loop_t* loop, struct uv__async* wa, uv__async_cb cb) {
  if (wa->wfd >=0)
    return 0;

  wa->wfd = 0;
  wa->cb = cb;
  return 0;
}


void uv__async_send(struct uv__async* wa) {
  wa->wfd = 2;
}


void uv__async_check(uv_loop_t* loop) {
  struct uv__async* wa = &loop->async_watcher;
  if (wa->wfd > 1) {
    wa->cb(loop, wa, 1);
    wa->wfd = 0;
  }
}