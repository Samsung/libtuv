/* Copyright 2014-2015 Samsung Electronics Co., Ltd.
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


/* Do platform-specific initialization. */
int platform_init() {
  return 0;
}

void run_sleep(int msec) {
  tuvp_msleep(msec);
}

int run_helper(task_entry_t* task) {
  (void)task;
  return 0;
}

int wait_helper(task_entry_t* task) {
  (void)task;
  return 0;
}

int run_test_one(task_entry_t* task) {
  int result;

  result = run_test_part(task->task_name, task->process_name);
  if (result) {
    if (result == 255) {
      // end of test
      ReleaseDebugSettings();
    }
    else {
      printf("!!! Failed to run [%s]\r\n", task->task_name);
    }
  }
  return 0;
}

void run_tests_continue(void) {
  minar::Scheduler::postCallback(run_tests_one);
}

int tuvtester_entry(void) {
  InitDebugSettings();

  platform_init();
  run_tests_init();

  minar::Scheduler::postCallback(run_tests_one);

  return 0;
}


//----------------------------------------------------------------------------

static void call_tuv_tester(void) {
  tuvtester_entry();
  printf("\r\ntuvtester_entry call OK\r\n");
}

static void blinky(void) {
  static DigitalOut led1(LED1);
  static DigitalOut led2(LED2);
  static DigitalOut led3(LED3);
  static int bl = 0;

  led1 = bl & 1 ? 0 : 1;
  led2 = bl & 2 ? 0 : 1;
  led3 = bl & 4 ? 0 : 1;

  bl++;

  if (bl < 16) {
    printf("LED bl(%d) %d %d %d\r\n",
            bl, led1.read(), led2.read(), led3.read());
    if (bl == 15) {
      printf("Too many message, stop...\r\n");
    }
  }
}

void app_start(int, char**){
  // set 115200 baud rate for stdout
  static Serial pc(USBTX, USBRX);
  pc.baud(115200);

  minar::Scheduler::postCallback(blinky)
                    .period(minar::milliseconds(1000))
                    .delay(minar::milliseconds(1000))
                    ;

  minar::Scheduler::postCallback(call_tuv_tester)
                    .delay(minar::milliseconds(500))
                    ;

  printf("\r\n\r\n app_start OK\r\n");
}
