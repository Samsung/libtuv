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


# override TUV_PLATFORM_PATH for Linux
# use "linux" for Linux platform dependent source files
set(TUV_PLATFORM_PATH ${PLATFORM_NAME_L})

# linux specific source files
set(LINUX_PATH "${SOURCE_ROOT}/${TUV_PLATFORM_PATH}")

set(PLATFORM_SRCFILES
      ${PLATFORM_SRCFILES}
      "${LINUX_PATH}/uv_linux.c"
      "${LINUX_PATH}/uv_linux_loop.c"
      "${LINUX_PATH}/uv_linux_clock.c"
      "${LINUX_PATH}/uv_linux_io.c"
      "${LINUX_PATH}/uv_linux_syscall.c"
      "${LINUX_PATH}/uv_linux_thread.c"
      )

set(PLATFORM_TESTFILES
      "${TEST_ROOT}/runner_linux.c"
      )

set(PLATFORM_HOSTHELPERFILES
      "${TEST_ROOT}/runner_linux_raw.c"
      )


set(TUV_LINK_LIBS "pthread")

if(DEFINED BUILD_HOST_HELPER AND BUILD_HOST_HELPER STREQUAL "yes")
  add_definitions("-D__HOST_HELPER__")
endif()
