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

#include <stdio.h>
#include <uv.h>

#include "raw_main.h" // in test/ folder

static DigitalOut led1(LED1);
static DigitalOut led2(LED2);
static DigitalOut led3(LED3);
static int bl = 0;

static void blinky_off(void) {
  led1 = 1;
  led2 = 1;
  led3 = 1;
}

static void blinky(void) {
  led1 = bl & 1 ? 0 : 1;
  led2 = bl & 2 ? 0 : 1;
  led3 = bl & 4 ? 0 : 1;
  bl++;

  minar::Scheduler::postCallback(blinky_off)
                    .delay(minar::milliseconds(100))
                    ;
}

/*
 * app_start() is entry function in mbed.
 * it calls call_tuv_tester() in libtuv raw_main.c
 * call_tuv_tester() will branch by build configuration; __TUV_APIEMUL__
 *
 * 1) __TUV_APIEMUL__: system api (socket, file) emulation test
 *    which calls apiemultester_entry() in apiemultester.cpp
 * 2) undefined: tuv api unit test
 *    which calls tuvtester_entry() in tuvtester.cpp
 */

void app_start(int, char**){
  static Serial pc(USBTX, USBRX);
  pc.baud(115200); // set 115200 baud rate for stdout

  minar::Scheduler::postCallback(blinky)
                    .period(minar::milliseconds(1000))
                    .delay(minar::milliseconds(2000))
                    ;

  minar::Scheduler::postCallback(call_tuv_tester)
                    .delay(minar::milliseconds(500))
                    ;

  printf("\r\n\r\n***** app_start() OK\r\n");
}
