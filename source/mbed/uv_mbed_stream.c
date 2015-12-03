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

#include <uv.h>


static void uv__stream_io(uv_loop_t* loop, uv__io_t* w, unsigned int events);


void uv__stream_init(uv_loop_t* loop, uv_stream_t* stream,
                     uv_handle_type type) {
  int err;

  uv__handle_init(loop, (uv_handle_t*)stream, type);
  stream->read_cb = NULL;
  stream->alloc_cb = NULL;
  stream->close_cb = NULL;
  stream->connection_cb = NULL;
  stream->connect_req = NULL;
  stream->shutdown_req = NULL;
  stream->accepted_fd = -1;
  stream->queued_fds = NULL;
  stream->delayed_error = 0;
  QUEUE_INIT(&stream->write_queue);
  QUEUE_INIT(&stream->write_completed_queue);
  stream->write_queue_size = 0;

  if (loop->emfile_fd == -1) {
    err = uv__open_cloexec("/", O_RDONLY);
    if (err >= 0)
      loop->emfile_fd = err;
  }

  uv__io_init(&stream->io_watcher, uv__stream_io, -1);
}


int uv__stream_open(uv_stream_t* stream, int fd, int flags) {
  assert(fd >= 0);
  stream->flags |= flags;

#pragma __TUV_MBED__ FIX THIS
#if 0
  if (stream->type == UV_TCP) {
    if ((stream->flags & UV_TCP_NODELAY) && uv__tcp_nodelay(fd, 1))
      return -get_errno();

    /* TODO Use delay the user passed in. */
    if ((stream->flags & UV_TCP_KEEPALIVE) && uv__tcp_keepalive(fd, 1, 60))
      return -get_errno();
  }
#endif

  stream->io_watcher.fd = fd;

  return 0;
}


//-----------------------------------------------------------------------------

void uv__server_io(uv_loop_t* loop, uv__io_t* w, unsigned int events) {
  (void)loop;
  (void)w;
  (void)events;
}


//-----------------------------------------------------------------------------

int uv_accept(uv_stream_t* server, uv_stream_t* client) {
  return -EINVAL;
}


int uv_listen(uv_stream_t* stream, int backlog, uv_connection_cb cb) {
  int err;

  switch (stream->type) {
  case UV_TCP:
    err = uv_tcp_listen((uv_tcp_t*)stream, backlog, cb);
    break;
/*
  case UV_NAMED_PIPE:
    err = uv_pipe_listen((uv_pipe_t*)stream, backlog, cb);
    break;
*/
  default:
    err = -EINVAL;
  }

  if (err == 0)
    uv__handle_start(stream);

  return err;
}


int uv_shutdown(uv_shutdown_t* req, uv_stream_t* stream, uv_shutdown_cb cb) {
  return 0;
}


//-----------------------------------------------------------------------------

static void uv__stream_io(uv_loop_t* loop, uv__io_t* w, unsigned int events) {
}
