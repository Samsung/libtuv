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

#include <errno.h>
#include <assert.h>

#include "mbed-drivers/mbed.h"

#include "tuv_mbed_port.h"

#include <uv.h>


ssize_t tuvp_readv(int fd, const struct iovec* iiovec, int count) {
  ssize_t result = 0;
  ssize_t total = 0;
  int idx;

  for (idx = 0; idx < count; ++idx) {
    result = tuvp_read(fd, iiovec[idx].iov_base, iiovec[idx].iov_len);
    if (result < 0) {
      return result;
    } else {
      total += result;
    }
  }
  return total;
}


ssize_t tuvp_writev(int fd, const struct iovec* iiovec, int count) {
  ssize_t result = 0;
  ssize_t total = 0;
  int idx;

  for (idx = 0; idx < count; ++idx) {
    result = tuvp_write(fd, iiovec[idx].iov_base, iiovec[idx].iov_len);
    if (result < 0) {
      return result;
    } else {
      total += result;
    }
  }
  return total;
}


int tuvp_poll(struct pollfd *fds, int nfds, int timeout) {
  (void)timeout;
  int netrets, filerets;

  filerets = 0;
  netrets = tuvp_net_poll(fds, nfds);

  return filerets + netrets;
}
