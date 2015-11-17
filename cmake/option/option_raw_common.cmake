# Copyright 2015 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(FLAGS_COMMON
      "-fno-builtin"
      "-D__TUV_RAW__"
      )

set(FLAGS_CXXONLY
      "-fpermissive"
      "-fno-exceptions"
      "-fno-rtti"
      )

set(CMAKE_C_FLAGS_DEBUG     "-O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE   "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# raw common source files
set(RAW_PATH "${SOURCE_ROOT}/raw")

set(PLATFORM_SRCFILES
      "${RAW_PATH}/uv_raw.c"
      "${RAW_PATH}/uv_raw_clock.c"
      "${RAW_PATH}/uv_raw_io.c"
      "${RAW_PATH}/uv_raw_loop.c"
      "${RAW_PATH}/uv_raw_threadpool.c"
      "${RAW_PATH}/uv_raw_thread.c"

      "${RAW_PATH}/uv_raw_async.c"
      "${RAW_PATH}/uv_raw_process.c"
      )

set(TEST_MAINFILE
      "${TEST_ROOT}/runner_main_raw.c"
      )

set(TEST_UNITFILES
      "${TEST_ROOT}/test_idle_raw.c"
      "${TEST_ROOT}/test_active_raw.c"
      "${TEST_ROOT}/test_timer_raw_init.c"
      "${TEST_ROOT}/test_timer_raw_norm.c"
      "${TEST_ROOT}/test_timer_raw_start_twice.c"
      "${TEST_ROOT}/test_timer_raw_order.c"
      "${TEST_ROOT}/test_timer_raw_run_once.c"
      "${TEST_ROOT}/test_timer_raw_run_null_callback.c"
      "${TEST_ROOT}/test_timer_raw_again.c"
      "${TEST_ROOT}/test_timer_raw_huge_timeout.c"
      "${TEST_ROOT}/test_timer_raw_huge_repeat.c"
      )