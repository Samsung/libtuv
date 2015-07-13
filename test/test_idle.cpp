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


void idle_callback(uv_idle_t* handle) {
  static int count = 0;
  printf("idle count = %d\n", count);
  if (count > 10)
    uv_idle_stop(handle);
  count++;
}


int main(int argc, char* argv[]) {
  uv_loop_t* loop;
  uv_idle_t idler;

#ifdef DEBUG
  printf("It's debug\n");
#endif

#ifdef NDEBUG
  printf("It's ndebug\n");
#endif

  loop = uv_default_loop();
  uv_idle_init(loop, &idler);
  uv_idle_start(&idler, idle_callback);
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}
