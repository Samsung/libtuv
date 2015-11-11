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

cmake_minimum_required(VERSION 2.8)

# platform name in lower case
string(TOLOWER ${CMAKE_SYSTEM_NAME} PLATFORM_NAME_L)

#
set(PATH_ROOT ${CMAKE_SOURCE_DIR})

set(SOURCE_ROOT  ${PATH_ROOT}/source)
set(INCLUDE_ROOT ${PATH_ROOT}/include)
set(TEST_ROOT    ${PATH_ROOT}/test)

if (DEFINED LIBTUV_CUSTOM_LIB_OUT)
  set(LIB_OUT ${LIBTUV_CUSTOM_LIB_OUT})
else()
  set(BIN_ROOT ${CMAKE_BINARY_DIR})
  set(LIB_OUT "${BIN_ROOT}/../lib")
  set(BIN_OUT "${BIN_ROOT}/../bin")
endif()

# path for platform depends files, use full name for default
# (e.g, i686-linux)
set(TUV_PLATFORM_PATH ${TARGET_PLATFORM})
include("cmake/option/option_${TARGET_PLATFORM}.cmake")

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_L)
if (${BUILD_TYPE_L} STREQUAL "debug")
  set(FLAGS_COMMON "${FLAGS_COMMON} -DENABLE_DEBUG_LOG")
endif()


string(REPLACE ";" " " CFLAGS_COMMON_STR "${CFLAGS_COMMON}")
set(FLAGS_COMMON "${FLAGS_COMMON} ${CFLAGS_COMMON_STR}")

foreach(FLAG ${FLAGS_COMMON})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

foreach(FLAG ${FLAGS_CXXONLY})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

message(STATUS "Build Type: [${CMAKE_BUILD_TYPE}]")
