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

#include "mbed-drivers/mbed.h"

#include "tuv_mbed_port.h"


int tuvp_task_create(tuvp_thread_t *thread,
                     tuv_thread_cb entry, tuv_thread_cb loop, void *arg) {

  minar::callback_handle_t handle;
  mbed::util::FunctionPointer1<void, void*> pstr(entry);
  mbed::util::FunctionPointer1<void, void*> ploop(loop);

  handle = minar::Scheduler::postCallback(pstr.bind(arg)).
                      delay(minar::milliseconds(5)).
                      getHandle();
  if (handle) {
    handle = minar::Scheduler::postCallback(ploop.bind(arg)).
                        delay(minar::milliseconds(10)).
                        period(minar::milliseconds(10)).
                        getHandle();
    if (handle) {
      *thread = (tuvp_thread_t)handle;
      return 0;
    }
  }
  return errno ? -errno : -ENOMEM;
}