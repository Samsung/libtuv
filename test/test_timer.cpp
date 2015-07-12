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

#include <stdio.h>

#include <uv.h>


void timer_callback1(uv_timer_t* handle) {
  static int callcount = 0;
  printf("timer callback1 %d\n", callcount);
  if (callcount >= 10) {
    uv_timer_stop(handle);
    printf("timer 1 stop\n");
  }
  callcount++;
}


void timer_callback2(uv_timer_t* handle) {
  static int callcount = 0;
  printf("timer callback2 %d\n", callcount);
  if (callcount >= 5) {
    uv_timer_stop(handle);
    printf("timer 2 stop\n");
  }
  callcount++;
}


int main(int argc, char* argv[]) {
  uv_loop_t* loop;
  uv_timer_t timer1;
  uv_timer_t timer2;

  loop = uv_default_loop();
  uv_timer_init(loop, &timer1);
  uv_timer_start(&timer1, timer_callback1, 1000, 200);
  uv_timer_init(loop, &timer2);
  uv_timer_start(&timer2, timer_callback2, 100, 500);

  printf("run loop\n");
  uv_run(loop, UV_RUN_DEFAULT);
  printf("end loop\n");

  uv_loop_close(loop);
}
