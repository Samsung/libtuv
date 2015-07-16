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

set(TEST_MAINFILE ${TEST_ROOT}/runner_main.cpp
                  ${TEST_ROOT}/runner_linux.cpp)

set(TEST_UNITFILES ${TEST_ROOT}/test_idle.cpp
                   ${TEST_ROOT}/test_timer.cpp
                   )

set(TESTEXENAME "tuvtester")

add_executable(${TESTEXENAME} ${TEST_MAINFILE} ${TEST_UNITFILES})
target_include_directories(${TESTEXENAME} PUBLIC ${LIB_TUV_INCDIRS})
target_link_libraries(${TESTEXENAME} LINK_PUBLIC "${TARGETLIBNAME}"
                      ${TUV_LINK_LIBS})
set_target_properties(${TESTEXENAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUT}"
    LIBRARY_OUTPUT_DIRECTORY "${LIB_OUT}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_OUT}")
