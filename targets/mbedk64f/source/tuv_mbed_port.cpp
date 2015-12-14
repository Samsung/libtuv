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

#include <errno.h>
#include <stdio.h>

#include "mbed-drivers/mbed.h"
#include "tuv_mbed_port.h"

#include <uv.h>

static int __platform_initialized = 0;


static void tuv__run_clear(uv_loop_t* loop) {
  loop->raw_loopcb = NULL;
  loop->raw_finalcb = NULL;
  loop->raw_param = NULL;
}


// TODO:
//  calling loop very 1msec is power consuming in mbed. need to fix this.
//  need to find a way to poll network event with timeout

static void handle_loopfinal(void* ploop) {

  uv_loop_t* loop;
  int result;

  loop = (uv_loop_t*)ploop;
  result = loop->raw_loopcb(loop->raw_param);

  if (result) {
    mbed::util::FunctionPointer1<void, void*> pstr(handle_loopfinal);
    minar::Scheduler::postCallback(pstr.bind(ploop))
                        .tolerance(minar::milliseconds(1))
                        .delay(minar::milliseconds(1))
                        ;
  }
  else {
    result = loop->raw_finalcb(loop->raw_param);
    if (result == 0)
      tuv__run_clear(loop);
  }
}


int tuv_run(uv_loop_t* loop, tuv_loop_cb loopcb, tuv_final_cb fincb,
            void* param) {

  loop->raw_loopcb = loopcb;
  loop->raw_finalcb = fincb;
  loop->raw_param = param;

  mbed::util::FunctionPointer1<void, void*> pstr(handle_loopfinal);
  minar::Scheduler::postCallback(pstr.bind(loop))
                      .tolerance(minar::milliseconds(1))
                      .delay(minar::milliseconds(1))
                      ;

  return 0;
}

void tuvp_platform_init(void) {
  if (__platform_initialized)
    return;

  __platform_initialized = 1;

  tuvp_tcp_init();
}


void exit(int status) {
  TDLOG("!!! EXIT !!! status(%d)\r\n\r\n", status);
  mbed_die();
  while(1);
}
