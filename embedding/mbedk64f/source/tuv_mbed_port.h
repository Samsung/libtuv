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

#ifndef __MBED_PORT_HEADER__
#define __MBED_PORT_HEADER__

#define POLLIN        0x01
#define POLLOUT       0x02
#define POLLERR       0x04
#define POLLHUP       0x08

#define PTHREAD_ONCE_INIT    0x12348765

#define AF_UNSPEC       0
#define AF_INET         2
#define PF_INET         AF_INET
#define PF_UNSPEC       AF_UNSPEC

#define IPPROTO_IP      0
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17
#define IPPROTO_UDPLITE 136


#ifdef __cplusplus
extern "C" {
#endif

#ifndef socklen_t
#  define socklen_t uint32_t
#endif

#if defined(TUV_DECLARE_MBED_DEFS)



/** For compatibility with BSD code */
struct in_addr {
  uint32_t s_addr;
};

/* members are in network byte order */
struct sockaddr_in {
  uint8_t sin_len;
  uint8_t sin_family;
  uint16_t sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct sockaddr {
  uint8_t sa_len;
  uint8_t sa_family;
  char    sa_data[14];
};

#endif // TUV_DECLARE_MBED_DEFS


//-----------------------------------------------------------------------------
// only for mbed apis

// timer
void tuvp_timer_init(void);
void tuvp_timer_start(void);
void tuvp_timer_restart(void);
void tuvp_timer_stop(void);
int tuvp_timer_usec(void);

// pipe
int tuvp_pipe(int fds[2]);

// thread emulation
typedef void* tuvp_thread_t;         // minar::callback_handle_t
typedef uint32_t tuvp_thread_attr_t;
typedef uint32_t tuvp_once_t;
typedef uint32_t tuvp_mutex_t;
typedef uint32_t tuvp_sem_t;
typedef uint32_t tuvp_cond_t;
typedef uint32_t tuvp_rwlock_t;

// should be same as uv_thread_cb
typedef void (*tuv_thread_cb)(void* arg);

int tuvp_task_create(tuvp_thread_t *thread,
                     tuv_thread_cb entry, tuv_thread_cb loop, void *arg);


#ifdef __cplusplus
}
#endif

#include <uv.h>


#endif // __MBED_PORT_HEADER__
