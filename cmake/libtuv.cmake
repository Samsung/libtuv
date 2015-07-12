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

set(ROOT ${CMAKE_SOURCE_DIR})

set(SOURCE_ROOT ${ROOT}/source)
set(INCLUDE_ROOT ${ROOT}/include)
set(TEST_ROOT ${ROOT}/test)

set(BIN_ROOT ${CMAKE_BINARY_DIR})
set(LIB_OUT "${BIN_ROOT}/../lib")
set(BIN_OUT "${BIN_ROOT}/../bin")

file(GLOB COMMON_SRCFILES "${SOURCE_ROOT}/*.cpp")
file(GLOB PLATFORM_SRCFILES "${SOURCE_ROOT}/${TUV_PLATFORM_PATH}/*.cpp")

set(LIB_TUV_SRCFILES
    ${COMMON_SRCFILES}
    ${PLATFORM_SRCFILES}
)

set(LIB_TUV_INCDIRS
    ${TARGET_INC}
    ${INCLUDE_ROOT}
    ${SOURCE_ROOT}
    ${SOURCE_ROOT}/${TUV_PLATFORM_PATH}
)

# build tuv library
set(TARGETLIBNAME tuv)
add_library(${TARGETLIBNAME} ${LIB_TUV_SRCFILES})
target_include_directories(${TARGETLIBNAME} PUBLIC ${LIB_TUV_INCDIRS})
set_target_properties(${TARGETLIBNAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUT}"
    LIBRARY_OUTPUT_DIRECTORY "${LIB_OUT}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_OUT}")

# build test executables
function(BuildTest testName testFile)
  set(TESTEXENAME ${testName})
  add_executable(${TESTEXENAME} "${TEST_ROOT}/${testFile}")
    target_include_directories(${TESTEXENAME} PUBLIC ${LIB_TUV_INCDIRS})
    target_link_libraries(${TESTEXENAME} LINK_PUBLIC ${TARGETLIBNAME})
    set_target_properties(${TESTEXENAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUT}"
        LIBRARY_OUTPUT_DIRECTORY "${LIB_OUT}"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_OUT}")
endfunction()

# build test executables
if(${BUILD_TEST} STREQUAL "YES")
  BuildTest("tuvidle"     "test_idle.cpp")
  BuildTest("tuvtimer"    "test_timer.cpp")
endif()
