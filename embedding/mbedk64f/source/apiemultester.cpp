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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <uv.h>

#include "apiemul.h"


#if APIEMUL_DUMP_HEAP_ADDRESS
static unsigned long lstack;

static void dump_heap_addr(void) {
  unsigned long result;
  unsigned long lheap1;
  unsigned long lheap2;
  void* heap1;
  uint32_t* heap2;

  heap1 = malloc(4);
  heap2 = new uint32_t;
  lheap1 = (unsigned long)heap1;
  lheap2 = (unsigned long)heap2;
  result = lstack - lheap1;
  delete heap2;
  free(heap1);

  TDDDLOG(":: heap info: s(%lx), h(%lx) h2(%lx), %ld",
          lstack, lheap1, lheap2, result);
}
#endif

int apiemultester_entry(void) {
  InitDebugSettings();

#if APIEMUL_DUMP_HEAP_ADDRESS
  char stackVariable;
  lstack = (unsigned long)&stackVariable;
  dump_heap_addr();
#endif

  minar::Scheduler::postCallback(apiemul_main)
                    .delay(minar::milliseconds(500))
                    ;

#if APIEMUL_DUMP_HEAP_ADDRESS
  minar::Scheduler::postCallback(dump_heap_addr)
                    .period(minar::milliseconds(5000))
                    ;
#endif

  return 0;
}
