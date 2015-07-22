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

/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdlib.h> // malloc(), free()
#include <stdio.h>

#include <uv.h>


//-----------------------------------------------------------------------------

int uv_rwlock_init(uv_rwlock_t* rwlock) {
  return -pthread_rwlock_init(rwlock, NULL);
}


void uv_rwlock_destroy(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_destroy(rwlock))
    abort();
}


void uv_rwlock_rdlock(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_rdlock(rwlock))
    abort();
}


int uv_rwlock_tryrdlock(uv_rwlock_t* rwlock) {
  int err;

  err = pthread_rwlock_tryrdlock(rwlock);
  if (err && err != EBUSY && err != EAGAIN)
    abort();

  return -err;
}


void uv_rwlock_rdunlock(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_unlock(rwlock))
    abort();
}


void uv_rwlock_wrlock(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_wrlock(rwlock))
    abort();
}


int uv_rwlock_trywrlock(uv_rwlock_t* rwlock) {
  int err;

  err = pthread_rwlock_trywrlock(rwlock);
  if (err && err != EBUSY && err != EAGAIN)
    abort();

  return -err;
}


void uv_rwlock_wrunlock(uv_rwlock_t* rwlock) {
  if (pthread_rwlock_unlock(rwlock))
    abort();
}

