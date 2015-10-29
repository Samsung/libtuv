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

# unix common source files
set(UNIX_PATH "${SOURCE_ROOT}/unix")

set(PLATFORM_SRCFILES "${UNIX_PATH}/uv_unix.c"
                      "${UNIX_PATH}/uv_unix_async.c"
                      "${UNIX_PATH}/uv_unix_io.c"
                      "${UNIX_PATH}/uv_unix_fs.c"
                      "${UNIX_PATH}/uv_unix_process.c"
                      "${UNIX_PATH}/uv_unix_thread.c"
                      "${UNIX_PATH}/uv_unix_tcp.c"
                      "${UNIX_PATH}/uv_unix_stream.c"
                      "${UNIX_PATH}/uv_unix_getaddrinfo.c"
                      )
