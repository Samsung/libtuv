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
#include <assert.h>

#include "mbed-drivers/mbed.h"

#include "tuv_mbed_port.h"

#include <uv.h>


typedef struct taskparam_t {
  tuv_taskentry_cb entry;
  tuv_taskloop_cb loop;
  void* data;
  minar::callback_handle_t hloop;
} taskparam;


static void task_entry(void *vparam) {
  taskparam* pparam = (taskparam*)vparam;
  if (pparam->entry) {
    pparam->entry(pparam->data);
  }
}


static void task_loop(void *vparam) {
  taskparam* pparam = (taskparam*)vparam;
  int ret = 0;
  if (pparam->loop) {
    ret = pparam->loop(pparam->data);
  }
  if (!ret) {
    if (pparam->loop)
      minar::Scheduler::cancelCallback(pparam->hloop);
    pparam->hloop = NULL;
  }
  else if (ret == -1) {
    if (pparam->loop)
      minar::Scheduler::cancelCallback(pparam->hloop);
    free(pparam);
  }
}


int tuvp_task_create(tuvp_thread_t *thread,
                     tuv_taskentry_cb entry, tuv_taskloop_cb loop, void *arg) {

  taskparam* pparam;

  pparam = (taskparam*)malloc(sizeof(taskparam));
  assert(pparam);
  pparam->entry = entry;
  pparam->loop = loop;
  pparam->data = arg;
  pparam->hloop = NULL;

  *thread = NULL;

  mbed::util::FunctionPointer1<void, void*> pstr(task_entry);
  mbed::util::FunctionPointer1<void, void*> ploop(task_loop);
  minar::callback_handle_t handle;

  handle = minar::Scheduler::postCallback(pstr.bind((void*)pparam)).
                      delay(minar::milliseconds(1)).
                      getHandle();
  if (handle) {
    pparam->hloop = minar::Scheduler::postCallback(ploop.bind((void*)pparam)).
                        delay(minar::milliseconds(10)).
                        period(minar::milliseconds(10)).
                        getHandle();
    if (pparam->hloop) {
      *thread = (tuvp_thread_t)pparam;
      return 0;
    }
    minar::Scheduler::cancelCallback(handle);
  }
  free(pparam);
  return errno ? -errno : -ENOMEM;
}


int tuvp_task_is_running(tuvp_thread_t thread) {
  if (!thread) return 0;
  taskparam* pparam = (taskparam*)thread;
  if (pparam->hloop) return 1;
  return 0;
}


int tuvp_task_close(tuvp_thread_t thread) {
  if (!thread) return -1;
  taskparam* pparam = (taskparam*)thread;
  if (pparam->hloop) {
    minar::Scheduler::cancelCallback(pparam->hloop);
  }
  free(pparam);
  return 0;
}
