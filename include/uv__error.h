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

#ifndef __uv__error_header__
#define __uv__error_header__

#ifndef __uv_header__
#error Please include with uv.h
#endif

#include <errno.h>

//-----------------------------------------------------------------------------
// errorno < uv_errorno.h

#define UV__EOF     (-4095)
#define UV__UNKNOWN (-4094)

#if defined(EAGAIN)
# define UV__EAGAIN (-EAGAIN)
#else
# define UV__EAGAIN (-4088)
#endif

#if defined(EBUSY)
# define UV__EBUSY (-EBUSY)
#else
# define UV__EBUSY (-4082)
#endif

#if defined(ECANCELED)
# define UV__ECANCELED (-ECANCELED)
#else
# define UV__ECANCELED (-4081)
#endif

#if defined(EINVAL)
# define UV__EINVAL (-EINVAL)
#else
# define UV__EINVAL (-4071)
#endif

#if defined(ENOMEM)
# define UV__ENOMEM (-ENOMEM)
#else
# define UV__ENOMEM (-4057)
#endif



//-----------------------------------------------------------------------------
// errno map < uv.h

#define UV_ERRNO_MAP(XX)                                                      \
  XX(EAGAIN, "resource temporarily unavailable")                              \
  XX(EBUSY, "resource busy or locked")                                        \
  XX(ECANCELED, "operation canceled")                                         \
  XX(EINVAL, "invalid argument")                                              \
  XX(ENOMEM, "not enough memory")                                             \


//-----------------------------------------------------------------------------
#define XX(code, _) UV_ ## code = UV__ ## code,

typedef enum {
  UV_ERRNO_MAP(XX)
  UV_ERRNO_MAX = UV__EOF - 1
} uv_errno_t;

#undef XX


#endif // __uv__error_header__
