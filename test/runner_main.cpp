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

#include <string.h>
#include <unistd.h>
#include <uv.h>

#include "runner.h"
#include "runner_list.h"


char executable_path[EXEC_PATH_LENGTH];


//-----------------------------------------------------------------------------
// test function declaration

#define TEST_DECLARE(name,x)                                                  \
  int run_test_##name(void);

TEST_LIST(TEST_DECLARE)

#undef TEST_DECLARE


//-----------------------------------------------------------------------------
// test function list

#define TEST_ENTRY(name, timeout) \
  { #name, #name, &run_test_##name, 0, 0, timeout },

task_entry_t TASKS[] = {

  TEST_LIST(TEST_ENTRY)

  { NULL, NULL, NULL, 0, 0, 0 }
};

#undef TEST_ENTRY

//-----------------------------------------------------------------------------

int run_test_part(const char* test, const char* part) {
  int r;
  task_entry_t* task;

  for (task = TASKS; task->main; task++) {
    if (strcmp(test, task->task_name) == 0 &&
        strcmp(part, task->process_name) == 0) {
      r = task->main();
      return r;
    }
  }

  fprintf(stderr, "No test part with that name: %s:%s\n", test, part);
  return 255;
}

int run_test_one(task_entry_t* task) {
  process_info_t processes;
  char errmsg[1024] = "no error";
  int result = 0;
  int status;

  status = 255;
  if (process_start(task->task_name,
                    task->process_name,
                    &processes, 0) == -1) {
    fprintf(stderr,
            "Process `%s` failed to start.",
            task->process_name);
    return -1;
  }

  usleep(1000);

  result = process_wait(&processes, 1, task->timeout);
  if (result == -1) {
    FATAL("process_wait failed ");
  } else if (result == -2) {
    /* Don't have to clean up the process, process_wait() has killed it. */
    fprintf(stderr, "timeout ");
    goto out;
  }
  status = process_reap(&processes);
  if (status != TEST_OK) {
    fprintf(stderr, "exit code %d ", status);
    result = -1;
    goto out;
  }

out:
  if (result == 0) {
    result = process_read_last_line(&processes, errmsg, sizeof(errmsg));
    if (strlen(errmsg))
      fprintf(stderr, "lastmsg:[%s] ", errmsg);
  }
  process_cleanup(&processes);

  return result;
}


int run_tests() {
  int entry = 0;
  int result;
  task_entry_t* task;

  while (TASKS[entry].task_name) {
    task = &TASKS[entry];

    fprintf(stderr, "[%-30s]...", task->task_name);
    result = run_test_one(task);
    fprintf(stderr, "%s\n", result ? "failed" : "OK");
    entry++;
  }
}


int main(int argc, char *argv[]) {
  int ret = -1;

  platform_init(argc, argv);

  switch (argc) {
  case 1:
    ret = run_tests();
    break;
  case 3:
    ret = run_test_part(argv[1], argv[2]);
    break;
  default :
    FATAL("System Error");
    break;
  }
  return ret;
}
