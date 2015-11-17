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

#include "mbed-drivers/mbed.h"
#include "sockets/TCPStream.h"

#include "tuv_mbed_port.h"


int tuvp_socket(int domain, int type, int protocol) {
  (void)domain;
  (void)type;
  (void)protocol;
  return 0;
}


int tuvp_accept(int s, struct sockaddr *addr, socklen_t *addrlen) {
  (void)s;
  (void)addr;
  (void)addrlen;
  return 0;
}


uint16_t tuvp_htons(uint16_t hostshort) {
  (void)hostshort;
  return 0;
}



