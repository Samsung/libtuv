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


#include <uv.h>

// only for mbed to start clock

static uint64_t __current_time = 0; // time in nano-seconds

void tuv__time_init(void) {
  if (__current_time == 0) {
    tuvp_timer_init();
    tuvp_timer_start();
    __current_time = 1000000; // at least should be bigger than this
  }
}


uint64_t uv__hrtime(uv_clocktype_t type) {
  int timevalue;

  // simple implementation with mbed clock, but there's an issue:
  // with 32bit integer, maximum of 2^31-1 microseconds i.e. 30 minutes
  // if this function is called after 30 minutes that time can be lost
  // so, this implementation assumes this function is called within
  // every 30 minutes.
  timevalue = tuvp_timer_usec();
  tuvp_timer_restart();
  __current_time += ((uint64_t)timevalue) * 1000; // to nano
  return __current_time;
}


inline uint64_t uv__time_precise() {
  return uv__hrtime(UV_CLOCK_PRECISE);
}
