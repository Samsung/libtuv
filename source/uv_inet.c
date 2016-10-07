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

/*
 * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
 * Copyright (c) 1996-1999 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <stdio.h>

#include <uv.h>

#define UV__INET_ADDRSTRLEN         16


//-----------------------------------------------------------------------------

static int inet_ntop4(const unsigned char *src, char *dst, size_t size) {
  static const char fmt[] = "%u.%u.%u.%u";
  char tmp[UV__INET_ADDRSTRLEN];
  int l;

  l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
  if (l <= 0 || (size_t) l >= size) {
    return UV_ENOSPC;
  }
  strncpy(dst, tmp, size);
  dst[size - 1] = '\0';
  return 0;
}


static int inet_pton4(const char *src, unsigned char *dst) {
  static const char digits[] = "0123456789";
  int saw_digit, octets, ch;
  unsigned char tmp[sizeof(struct in_addr)], *tp;

  saw_digit = 0;
  octets = 0;
  *(tp = tmp) = 0;
  while ((ch = *src++) != '\0') {
    const char *pch;

    if ((pch = strchr(digits, ch)) != NULL) {
      unsigned int nw = *tp * 10 + (pch - digits);

      if (saw_digit && *tp == 0) {
        return UV_EINVAL;
      }
      if (nw > 255) {
        return UV_EINVAL;
      }
      *tp = nw;
      if (!saw_digit) {
        if (++octets > 4) {
          return UV_EINVAL;
        }
        saw_digit = 1;
      }
    } else if (ch == '.' && saw_digit) {
      if (octets == 4) {
        return UV_EINVAL;
      }
      *++tp = 0;
      saw_digit = 0;
    } else {
      return UV_EINVAL;
    }
  }
  if (octets < 4) {
    return UV_EINVAL;
  }
  memcpy(dst, tmp, sizeof(struct in_addr));
  return 0;
}


//-----------------------------------------------------------------------------

int uv_inet_ntop(int af, const void* src, char* dst, size_t size) {
  switch (af) {
  case AF_INET:
    return (inet_ntop4((const unsigned char*)src, dst, size));
/*
  case AF_INET6:
    return (inet_ntop6(src, dst, size));
*/
  default:
    return UV_EAFNOSUPPORT;
  }
  /* NOTREACHED */
}


int uv_inet_pton(int af, const char* src, void* dst) {
  if (src == NULL || dst == NULL) {
    return UV_EINVAL;
  }

  switch (af) {
  case AF_INET:
    return (inet_pton4(src, (unsigned char*)dst));
/*
  case AF_INET6: {
    int len;
    char tmp[UV__INET6_ADDRSTRLEN], *s, *p;
    s = (char*) src;
    p = strchr(src, '%');
    if (p != NULL) {
      s = tmp;
      len = p - src;
      if (len > UV__INET6_ADDRSTRLEN-1) {
        return UV_EINVAL;
      }
      memcpy(s, src, len);
      s[len] = '\0';
    }
    return inet_pton6(s, dst);
  }
*/
  default:
    return UV_EAFNOSUPPORT;
  }
  /* NOTREACHED */
}
