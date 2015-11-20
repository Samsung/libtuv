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
// thread emulation with minar::scheduler


int tuv_task_create(uv_thread_t *tid,
                    tuv_taskentry_cb entry, tuv_taskloop_cb loop, void *arg) {

  return tuvp_task_create(tid, entry, loop, arg);
}


int tuv_task_running(uv_thread_t *tid) {
  if (!tid) return 0;
  // waiting for the thread to join is not possible for raw systems
  // if the tid is valid and running, return 1,
  // if invalid(null) return 0
  return tuvp_task_is_running(*tid);
}

int tuv_task_close(uv_thread_t *tid) {
  // free internal task related memory allocations if any
  return tuvp_task_close(*tid);
}


int uv_thread_equal(const uv_thread_t* t1, const uv_thread_t* t2) {
  return 1;
}


//-----------------------------------------------------------------------------
// once

void uv_once(uv_once_t* guard, void (*callback)(void)) {
  if (*guard == UV_ONCE_INIT) {
    callback();
    *guard = ~UV_ONCE_INIT;
  }
}


//-----------------------------------------------------------------------------
// mutex

int uv_mutex_init(uv_mutex_t* mutex) {
  *mutex = 1;
  return 0;
}


void uv_mutex_destroy(uv_mutex_t* mutex) {
  *mutex = 0;
}


void uv_mutex_lock(uv_mutex_t* mutex) {
  (*mutex)++;
}


void uv_mutex_unlock(uv_mutex_t* mutex) {
  if(*mutex > 1)
    *(mutex)--;
}


//-----------------------------------------------------------------------------
// semaphore

int uv_sem_init(uv_sem_t* sem, unsigned int value) {
  *sem = value;
  return 0;
}


void uv_sem_destroy(uv_sem_t* sem) {
  *sem = 0;
}


void uv_sem_post(uv_sem_t* sem) {
  (*sem)++;
}


void uv_sem_wait(uv_sem_t* sem) {
  (*sem)--;
}



//-----------------------------------------------------------------------------
// condition emulation
// 1 is inital value.
// 0 when destroyed
// 2 when signaled
// 9 when broadcasted

int uv_cond_init(uv_cond_t* cond) {
  *cond = 1;
  return 0;
}

void uv_cond_destroy(uv_cond_t* cond) {
  *cond = 0;
}

void uv_cond_signal(uv_cond_t* cond) {
  *cond = 2;
}

void uv_cond_broadcast(uv_cond_t* cond) {
  *cond = 9;
}

int tuv_cond_wait(uv_cond_t* cond, uv_mutex_t* mutex) {
  int ret = (*cond > 1) ? 1 : 0;
  *cond = 1;
  return ret;
}

int tuv_cond_timedwait(uv_cond_t* cond, uv_mutex_t* mutex, uint64_t timeout) {
  // how can we treat this raw(mbed) system?
  int ret = (*cond > 1) ? 1 : 0;
  *cond = 1;
  return ret;
}


//-----------------------------------------------------------------------------

int uv_rwlock_init(uv_rwlock_t* rwlock) {
  return 0;
}


void uv_rwlock_destroy(uv_rwlock_t* rwlock) {
}


void uv_rwlock_rdlock(uv_rwlock_t* rwlock) {
}


void uv_rwlock_rdunlock(uv_rwlock_t* rwlock) {
}


void uv_rwlock_wrlock(uv_rwlock_t* rwlock) {
}


void uv_rwlock_wrunlock(uv_rwlock_t* rwlock) {
}
