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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <uv.h>

#include "runner.h"
#include "raw_main.h"


/* Do platform-specific initialization. */
int platform_init() {
  return 0;
}

void run_sleep(int msec) {
  uv_sleep(msec);
}

int run_helper(task_entry_t* task) {
  task->main();
  return 0;
}

int wait_helper(task_entry_t* task) {
  TDDDLOG("wait_helper... not implemented");
  (void)task;
  return 0;
}

static void call_cleanup(void) {
  tuv_cleanup();
}

int run_test_one(task_entry_t* task) {
  int result;

  if (task) {
    result = run_test_part(task->task_name, task->process_name);
  }
  else {
    result = 255;
  }

  if (result) {
    if (result == 255) {
      // end of test
      minar::Scheduler::postCallback(call_cleanup)
                        .delay(minar::milliseconds(1))
                        ;
    }
    else {
      TDLOG("!!! Failed to run [%s]", task->task_name);
    }
  }
  return 0;
}

void run_tests_continue(void) {
  minar::Scheduler::postCallback(run_tests_one);
}

void tuvtester_entry(void) {
  InitDebugSettings();

  platform_init();
  run_tests_init();

  minar::Scheduler::postCallback(run_tests_one);
}
