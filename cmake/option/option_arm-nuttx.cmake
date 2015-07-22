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
      "-D__NUTTX__"
      "-fno-builtin"
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

# override TUV_PLATFORM_PATH for NuttX
# use "nuttx" for NuttX platform dependent source files
set(TUV_PLATFORM_PATH ${PLATFORM_NAME_L})

# unix common source files
set(UNIX_PATH "${SOURCE_ROOT}/unix")

set(PLATFORM_SRCFILES "${UNIX_PATH}/uv_unix.cpp"
                      "${UNIX_PATH}/uv_unix_async.cpp"
                      "${UNIX_PATH}/uv_unix_io.cpp"
                      "${UNIX_PATH}/uv_unix_process.cpp"
                      "${UNIX_PATH}/uv_unix_thread.cpp"
                      )

# nuttx source files
set(NUTTX_PATH "${SOURCE_ROOT}/${TUV_PLATFORM_PATH}")

set(PLATFORM_SRCFILES ${PLATFORM_SRCFILES}
                      "${NUTTX_PATH}/uv_nuttx.cpp"
                      "${NUTTX_PATH}/uv_nuttx_clock.cpp"
                      "${NUTTX_PATH}/uv_nuttx_io.cpp"
                      "${NUTTX_PATH}/uv_nuttx_loop.cpp"
                      "${NUTTX_PATH}/uv_nuttx_thread.cpp"
                      )

set(PLATFORM_TESTFILES "${TEST_ROOT}/runner_nuttx.cpp")

if(DEFINED TARGET_BOARD)
  if(${TARGET_BOARD} STREQUAL "stm32f4disco")
    set(FLAGS_COMMON
          "${FLAGS_COMMON}"
          "-mcpu=cortex-m4"
          "-mthumb"
          "-march=armv7e-m"
          "-mfpu=fpv4-sp-d16"
          "-mfloat-abi=hard"
          "-mfloat-abi=hard"
          )
  else()
    message(STATUS "WARNING: TARGET_BOARD=`${TARGET_BOARD}` is unknown to make")
  endif()
endif()

if(NOT DEFINED TARGET_SYSTEMROOT OR "${TARGET_SYSTEMROOT}" STREQUAL "default")
  message(FATAL_ERROR "\nPlease set TARGET_SYSTEMROOT for NuttX include path\n")
endif()

# system include folder
set(TARGET_INC ${TARGET_INC} "${TARGET_SYSTEMROOT}/include")
set(TARGET_INC ${TARGET_INC} "${TARGET_SYSTEMROOT}/include/cxx")

# build text as library
set(BUILD_TEST_LIB "YES")
unset(BUILD_TEST_LIB CACHE)

# set copy libs to ${TARGET_SYSTEMROOT}/lib
set(COPY_TARGET_LIB "${TARGET_SYSTEMROOT}/lib")
