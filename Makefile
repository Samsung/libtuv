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

TUV_PLATFORM    ?= i686-linux
TUV_BUILD_TYPE  ?= debug
TUV_BOARD       ?= unknown
TUV_BUILDTEST   ?= YES

OUTPUT_ROOT     := build
BUILD_FOLDER    := ./$(OUTPUT_ROOT)/$(TUV_PLATFORM)/$(TUV_BUILD_TYPE)
CMAKE_FOLDER    := $(BUILD_FOLDER)/cmake
CMAKE_DEFINES   := \
	-DCMAKE_TOOLCHAIN_FILE=./cmake/config/config_$(TUV_PLATFORM).cmake \
	-DCMAKE_BUILD_TYPE=$(TUV_BUILD_TYPE) \
	-DTARGET_PLATFORM=$(TUV_PLATFORM) \
	-DBUILD_TEST=$(TUV_BUILDTEST)

ifneq ($(TUV_BOARD),unknown)
	CMAKE_DEFINES += -DTARGET_BOARD=${TUV_BOARD}
endif

.phony: all

all:
	mkdir -p $(CMAKE_FOLDER)
	cmake -B$(CMAKE_FOLDER) -H./ $(CMAKE_DEFINES)
	make -C $(CMAKE_FOLDER)
	@echo '=============================================================='
	@echo 'to run test,'
	@echo '$(BUILD_FOLDER)/bin/tuvtester'
	@echo '=============================================================='

call:
	make -C $(CMAKE_FOLDER)

clean:
	rm -rf $(OUTPUT_ROOT)

#
# make option
#   VERBOSE=1
#       ; verbose output if set
#   TUV_BUILD_TYPE=release
#       ; release build or else debug build
#
# support platform+board
#   TUV_PLATFORM=i686-linux
#       ; x86 32bit linux
#   TUV_PLATFORM=x86_64-linux
#       ; x86 32bit linux
#   TUV_PLATFORM=arm-linux TUV_BOARD=rpi2
#       ; arm linux raspberry pi 2
#
