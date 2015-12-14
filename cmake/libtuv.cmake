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

# temporary block to make one file at a time for mbed
if(${PLATFORM_NAME_L} STREQUAL "mbed")
  set(COMMON_SRCFILES
        "${SOURCE_ROOT}/uv_handle.c"
        "${SOURCE_ROOT}/uv_loop.c"
        "${SOURCE_ROOT}/uv_idle.c"
        "${SOURCE_ROOT}/uv_run.c"
        "${SOURCE_ROOT}/uv_timer.c"
        "${SOURCE_ROOT}/uv_req.c"

        "${SOURCE_ROOT}/uv_async.c"
        "${SOURCE_ROOT}/uv_util.c"
        "${SOURCE_ROOT}/tuv_debuglog.c"
        "${SOURCE_ROOT}/uv_error.c"

        "${SOURCE_ROOT}/uv_inet.c"
        )
else()
  set(COMMON_SRCFILES
        "${SOURCE_ROOT}/uv_handle.c"
        "${SOURCE_ROOT}/uv_loop.c"
        "${SOURCE_ROOT}/uv_idle.c"
        "${SOURCE_ROOT}/uv_run.c"
        "${SOURCE_ROOT}/uv_timer.c"
        "${SOURCE_ROOT}/uv_req.c"
        "${SOURCE_ROOT}/uv_fs.c"
        "${SOURCE_ROOT}/uv_async.c"
        "${SOURCE_ROOT}/uv_util.c"
        "${SOURCE_ROOT}/tuv_debuglog.c"
        "${SOURCE_ROOT}/uv_error.c"
        "${SOURCE_ROOT}/uv_dir.c"
        "${SOURCE_ROOT}/uv_inet.c"
        )
endif()

set(LIB_TUV_SRCFILES
      ${COMMON_SRCFILES}
      ${PLATFORM_SRCFILES}
      )

set(LIB_TUV_INCDIRS
      ${INCLUDE_ROOT}
      ${SOURCE_ROOT}
      ${SOURCE_ROOT}/${TUV_PLATFORM_PATH}
      )


# build tuv library
set(TARGETLIBNAME tuv)
add_library(${TARGETLIBNAME} ${LIB_TUV_SRCFILES})
target_include_directories(${TARGETLIBNAME} SYSTEM PRIVATE ${TARGET_INC})
target_include_directories(${TARGETLIBNAME} PUBLIC ${LIB_TUV_INCDIRS})
set_target_properties(${TARGETLIBNAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUT}"
    LIBRARY_OUTPUT_DIRECTORY "${LIB_OUT}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_OUT}")

if(DEFINED COPY_TARGET_LIB)
  add_custom_command(TARGET ${TARGETLIBNAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGETLIBNAME}>
                                  "${COPY_TARGET_LIB}"
      COMMENT "Copying lib${TARGETLIBNAME} to ${COPY_TARGET_LIB}")
endif()
