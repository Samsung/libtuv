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

#include "mbed-drivers/mbed.h"

#include "tuv_mbed_port.h"

#include <uv.h>


void uv_sleep(int msec) {
  wait_ms(msec);
}

void tuv_usleep(int usec) {
  wait_us(usec);
}


//-----------------------------------------------------------------------------

int tuvp_pipe(int fds[2]) {
  // TODO: implement this
  fds[0] = 1;
  fds[1] = 2;
  return 0;
}