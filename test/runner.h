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

#ifndef __tuv_test_runner_header__
#define __tuv_test_runner_header__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  const char *task_name;
  const char *process_name;
  int (*main)(void);
  int is_helper;
  int show_output;

  /*
   * The time in milliseconds after which a single test or benchmark times out.
   */
  int timeout;
} task_entry_t, bench_entry_t;


#define TEST_IMPL(name)                                                       \
  int run_test_##name(void);                                                  \
  int run_test_##name(void)

/* Have our own assert, so we are sure it does not get optimized away in
 * a release build.
 */
#define ASSERT(expr)                                      \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: %s\n",    \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
    abort();                                              \
  }                                                       \
 } while (0)

#define WARN(expr)                                        \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf(stderr,                                       \
            "Failed in %s on line %d: %s\n",              \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
    return -1;                                            \
  }                                                       \
 } while (0)

#define FATAL(msg)                                        \
  do {                                                    \
    fprintf(stderr,                                       \
            "Fatal error in %s on line %d: %s\n",         \
            __FILE__,                                     \
            __LINE__,                                     \
            msg);                                         \
    fflush(stderr);                                       \
    abort();                                              \
  } while (0)

/* Reserved test exit codes. */
enum test_status {
  TEST_OK = 0,
  TEST_TODO,
  TEST_SKIP
};

typedef struct {
  FILE* stdout_file;
  pid_t pid;
  char* name;
  int status;
  int terminated;
} process_info_t;

typedef struct {
  int pipe[2];
  process_info_t* vec;
  int n;
} dowait_args;


#ifdef PATH_MAX
#define EXEC_PATH_LENGTH PATH_MAX
#else
#define EXEC_PATH_LENGTH 4096
#endif

extern char executable_path[EXEC_PATH_LENGTH];


int platform_init(int argc, char **argv);

int process_start(const char* name, const char* part, process_info_t* p,
                  int is_helper);
int process_wait(process_info_t* vec, int n, int timeout);
int process_reap(process_info_t *p);
int process_read_last_line(process_info_t *p, char* buffer, size_t buffer_len);
void process_cleanup(process_info_t *p);

#endif // __tuv_test_runner_header__
