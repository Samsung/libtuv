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
#include "sal-iface-eth/EthernetInterface.h"
#include "sal-stack-lwip/lwipv4_init.h"
#include "sockets/TCPAsynch.h"
#include "sockets/TCPStream.h"
#include "sockets/TCPListener.h"

#include "tuv_mbed_port.h"

#include <uv.h>

// To use static IP Address defined in the source code, add
// tuv_mbed_ipaddress.h file that looks like below. If tuv_mbed_ipaddress.h
// file exists, __TUV_MBED_IPEXIST__ is defined in maketuvtester.cmake.
// After you add this header file, you need to do a clean build something like;
// $ cd targets/mbedk64f
// $ yotta clean
/*
#ifndef __tuv_mbed_ipaddress_header__
#define __tuv_mbed_ipaddress_header__

#define MBED_IP_ADDRESS   "192.168.1.100"
#define MBED_IP_MASK      "255.255.255.0"
#define MBED_IP_GATEWAY   "192.168.1.1"

#endif
*/
#if defined(__TUV_MBED_IPEXIST__)
#include "tuv_mbed_ipaddress.h"
#endif

using namespace mbed::Sockets::v0;

//-----------------------------------------------------------------------------

static EthernetInterface _eth;

//-----------------------------------------------------------------------------

#ifndef GETMIN
#define GETMIN(A,B) ((A)>(B)?(B):(A))
#endif

#define MBED_SOCKET_FLAG_NONE           0x0000
#define MBED_SOCKET_FLAG_LISTENER       0x0001
#define MBED_SOCKET_FLAG_STREAM         0x0002
#define MBED_SOCKET_FLAG_CONNECTED      0x0010
#define MBED_SOCKET_FLAG_CONNECTING     0x0020  // for stream
#define MBED_SOCKET_FLAG_CONNECTFAILED  0x0040  // for stream
#define MBED_SOCKET_FLAG_CONNECTION     0x0100  // for listener
#define MBED_SOCKET_FLAG_DISCONNECTED   0x0200
#define MBED_SOCKET_FLAG_READABLE       0x0400
#define MBED_SOCKET_FLAG_WRITABLE       0x0800


/*
 * class mbed_socket
 *    to implement BSD socket like apis with mbed Sockets class
*/

class mbed_socket {
protected:
  mbed_socket();
  mbed_socket(int domain, int type, int protocol);

  void init(void);
  void release(void);

public:
  int dobind(const struct sockaddr *addr, socklen_t addrlen);
  int dolisten(int backlog);
  int doaccept(struct sockaddr *addr, socklen_t *addrlen);
  int doconnect(const struct sockaddr *addr, socklen_t addrlen);
  int dopoll(struct pollfd* fds);
  int dogetsockname(struct sockaddr* addr, socklen_t* addrlen);
  int dogetpeername(struct sockaddr* addr, socklen_t* addrlen);
  ssize_t dowrite(const void* buf, size_t count);
  ssize_t doread(void *buf, size_t count);
  int dogetsockopt(int level, int optname, void *optval, socklen_t *optlen);

  void doclose(void); // delayed close

  typedef TCPStream::ConnectHandler_t on_connect_t;
  typedef TCPStream::DisconnectHandler_t on_disconnect_t;
  typedef Socket::ErrorHandler_t on_error_t;
  typedef Socket::ReadableHandler_t on_readable_t;
  typedef Socket::SentHandler_t on_sent_t;


protected:
  int get_free_slot(void);
  int get_sock_fd(void);

  void set_stream(TCPStream *s, bool connected) {
    tcp_.stream = s;
    sock_flag_ |= MBED_SOCKET_FLAG_STREAM;
    if (connected) {
      sock_flag_ |= MBED_SOCKET_FLAG_WRITABLE;
      sock_flag_ |= MBED_SOCKET_FLAG_CONNECTED;
    }
  }
  void clear_stream(TCPStream *s) {
    (void)s;
    assert(s == tcp_.stream);
    sock_flag_ &= ~MBED_SOCKET_FLAG_STREAM;
    sock_flag_ &= ~MBED_SOCKET_FLAG_WRITABLE;
    sock_flag_ &= ~MBED_SOCKET_FLAG_CONNECTED;
    tcp_.stream = NULL;
  }
  void set_listener(TCPListener* l) {
    tcp_.listener = l;
    sock_flag_ |= MBED_SOCKET_FLAG_LISTENER;
  }

  int incomming_push(void* impl);
  void* incomming_pop(void);

  void on_incoming(TCPListener *s, void *impl);
  void on_error(Socket *s, socket_error_t err);
  void on_readable(Socket *s);
  void on_sent(Socket *s, uint16_t nbytes);
  void on_connect(TCPStream *s);
  void on_disconnect(TCPStream *s);

  int release_slot(void);

public:
  TCPListener* get_listener(void) {
    if (sock_flag_ & MBED_SOCKET_FLAG_LISTENER) {
      return tcp_.listener;
    }
    return NULL;
  }
  int get_incomming() {
    return incomming_cnt_;
  }
  TCPStream* get_stream(void) {
    if (sock_flag_ & MBED_SOCKET_FLAG_STREAM) {
      return tcp_.stream;
    }
    return NULL;
  }

protected:
  union {
    TCPAsynch* tcp;
    TCPStream* stream;
    TCPListener* listener;
  } tcp_;
  int sock_fd_;
  unsigned int sock_flag_;

  int domain_;
  int type_;
  int protocol_;

  void** incomming_;
  int incomming_cnt_;
  int incomming_max_;

protected:
  static mbed_socket* sockets_[TUV_MAX_SD_COUNT];

public:
  static int slot_to_fd(int slot);
  static int fd_to_slot(int fd);
  static mbed_socket* fd_to_so(int sockfd);

public:
  static int create(int domain, int type, int protocol);
  static int check_poll(void);
  static void init_sockets(void);
};


//-----------------------------------------------------------------------------
// buffer for sockets objects
// descriptor number is slot number + TUV_MAX_FF_COUNT + TUV_MAX_FD_COUNT
//  TUV_MAX_FF_COUNT is offset to no-meaning number
//  TUV_MAX_FD_COUNT is amount of file descriptors

mbed_socket* mbed_socket::sockets_[TUV_MAX_SD_COUNT];


//-----------------------------------------------------------------------------


int mbed_socket::slot_to_fd(int slot) {
  return slot + TUV_MAX_FF_COUNT + TUV_MAX_FD_COUNT;
}


int mbed_socket::fd_to_slot(int fd) {
  return fd - TUV_MAX_FF_COUNT - TUV_MAX_FD_COUNT;
}


mbed_socket* mbed_socket::fd_to_so(int sockfd) {
  int slot = fd_to_slot(sockfd);
  if (slot < 0 || slot >= TUV_MAX_SD_COUNT)
    return NULL;
  return sockets_[slot];
}


void mbed_socket::init_sockets(void) {
  int i;
  for (i=0; i<TUV_MAX_SD_COUNT; i++) {
    sockets_[i] = NULL;
  }
}


int mbed_socket::create(int domain, int type, int protocol) {
  mbed_socket* so;
  so = new mbed_socket(domain, type, protocol);
  if (so == NULL) {
    errno = ENOMEM;
    return -1;
  }
  int sock_fd;
  sock_fd = so->get_sock_fd();
  if (sock_fd < 0) {
    delete so;
    errno = ENOMEM;
    return -1;
  }
  return sock_fd;
}


//-----------------------------------------------------------------------------

mbed_socket::mbed_socket() {
  init();
}


mbed_socket::mbed_socket(int domain, int type, int protocol) {
  init();

  domain_ = domain;
  type_ = type;
  protocol_ = protocol;
}


void mbed_socket::init(void) {
  tcp_.tcp = NULL;
  sock_fd_ = 0;
  sock_flag_ = MBED_SOCKET_FLAG_NONE;

  domain_ = 0;
  type_ = 0;
  protocol_ = 0;

  incomming_ = NULL;
  incomming_cnt_ = 0;
  incomming_max_ = 0;
}


int mbed_socket::get_free_slot(void) {
  int i;
  for (i=0; i<TUV_MAX_SD_COUNT; i++) {
    if (sockets_[i] == 0) {
      return i;
    }
  }
  return -1;
}


int mbed_socket::get_sock_fd(void) {
  if (sock_fd_ > 0) {
    return sock_fd_;
  }

  int slot = get_free_slot();
  if (slot < 0) {
    return -1;
  }
  sockets_[slot] = this;
  sock_fd_ = slot_to_fd(slot);
  return sock_fd_;
}


int mbed_socket::release_slot(void) {
  if (sock_fd_ < 0) {
    TDLOG(".. release_slot invalid sockfd");
    return -1;
  }

  int slot = fd_to_slot(sock_fd_);
  if (slot < 0 || slot >= TUV_MAX_SD_COUNT) {
    TDLOG(".. release_slot invalid slot");
    return -1;
  }
  sockets_[slot] = NULL;
  return 0;
}


int mbed_socket::incomming_push(void* impl) {
  int i;

  //TDDDLOG(".. incomming_push max(%d), cnt(%d)",
  //        incomming_max_, incomming_cnt_);
  for (i=0; i<incomming_max_; i++) {
    if (incomming_[i] == NULL) {
      incomming_[i] = impl;
      incomming_cnt_++;
      sock_flag_ |= MBED_SOCKET_FLAG_CONNECTION;
      return i;
    }
  }
  return -1;
}


void* mbed_socket::incomming_pop(void) {
  int i;
  void* impl;
  // TODO: fix this to FIFO
  for (i=0; i<incomming_max_; i++) {
    if (incomming_[i] != NULL) {
      impl = incomming_[i];
      incomming_[i] = NULL;
      incomming_cnt_--;
      if (!incomming_cnt_)
        sock_flag_ &= ~MBED_SOCKET_FLAG_CONNECTION;
      return impl;
    }
  }
  sock_flag_ &= ~MBED_SOCKET_FLAG_CONNECTION;
  return NULL;
}


void mbed_socket::on_error(Socket *s, socket_error_t err) {
  (void)s;
  (void)err;
  TDLOG("socket(%p) Error: %s (%d)", s, socket_strerror(err), err);
  TDLOG("  stream(%p), listener(%p)", get_stream(), get_listener());
  TDLOG("  flag(%x)", sock_flag_);

  if (err == 21 && (sock_flag_ & MBED_SOCKET_FLAG_CONNECTING)) {
    set_errno(ECONNREFUSED);
    sock_flag_ &= ~MBED_SOCKET_FLAG_CONNECTING;
    sock_flag_ |= MBED_SOCKET_FLAG_CONNECTFAILED;
  }
}


void mbed_socket::on_readable(Socket *s) {
  (void)s;
  //TDDDLOG(".. on_readable Socket(%p), stream(%p)", s, get_stream());
  assert(s == get_stream());
  sock_flag_ |= MBED_SOCKET_FLAG_READABLE;
  set_errno(0);
}


void mbed_socket::on_sent(Socket *s, uint16_t nbytes) {
  (void)s;
  (void)nbytes;
  //TDDDLOG(".. on_sent Socket(%p), stream(%p) nbytes(%d)",
  //        s, get_stream(), nbytes);
  assert(s == get_stream());
  sock_flag_ |= MBED_SOCKET_FLAG_WRITABLE;
  set_errno(0);
}


void mbed_socket::on_connect(TCPStream *s) {
  (void)s;
  //TDDDLOG(".. on_connect fd(%d), TCPStream(%p), _stream(%p)",
  //        sock_fd_, s, get_stream());
  assert(s == get_stream());
  sock_flag_ |= MBED_SOCKET_FLAG_CONNECTED;
  sock_flag_ |= MBED_SOCKET_FLAG_WRITABLE;
  sock_flag_ &= ~MBED_SOCKET_FLAG_CONNECTING;
  set_errno(0);
}


void mbed_socket::on_disconnect(TCPStream *s) {
  (void)s;
  //TDDDLOG(".. on_disconnect TCPStream(%p), _stream(%p)", s, get_stream());
  assert(s == get_stream());
  sock_flag_ |= MBED_SOCKET_FLAG_DISCONNECTED;
  sock_flag_ &= ~MBED_SOCKET_FLAG_CONNECTED;
  sock_flag_ &= ~MBED_SOCKET_FLAG_WRITABLE;
}


//-----------------------------------------------------------------------------

void mbed_socket::on_incoming(TCPListener *s, void *impl) {
  if (impl == NULL) {
    on_error(s, SOCKET_ERROR_NULL_PTR);
    return;
  }
  //TDDDLOG(".. on_incoming this(%p) s(%p) impl(%p)", this, s, impl);
  if (incomming_push(impl) < 0) {
    on_error(s, SOCKET_ERROR_BAD_ALLOC);
    return;
  }
}


//-----------------------------------------------------------------------------

static void makeaddr4string(char* addrstr, struct sockaddr_in* inaddr) {
  union {
    uint8_t addr8[4];
    uint32_t addr32;
  } c4;
  c4.addr32 = inaddr->sin_addr.s_addr;
  if (addrstr) {
    sprintf(addrstr,
            "%d.%d.%d.%d",
            c4.addr8[0], c4.addr8[1], c4.addr8[2], c4.addr8[3]);
  }
  //printf(".. addr [%08lx][%d.%d.%d.%d:%d]\r\n",
  //       inaddr->sin_addr.s_addr,
  //       c4.addr8[0], c4.addr8[1], c4.addr8[2], c4.addr8[3],
  //       inaddr->sin_port);
}


//-----------------------------------------------------------------------------

int mbed_socket::dobind(const struct sockaddr *addr, socklen_t addrlen) {
  (void)addrlen;
  if (tcp_.listener != NULL) {
    // already `bind`ed or it's a stream
    TDLOG(".. dobind error: listener already exist");
    set_errno(EINVAL);
    return -1;
  }
  TCPListener* listener;
  socket_error_t err;

  listener = new TCPListener(SOCKET_STACK_LWIP_IPV4);
  err = listener->open(SOCKET_AF_INET4);
  if (listener->error_check(err)) {
    TDLOG(".. dobind error: open failed %s (%d)",
           socket_strerror(err), err);
    set_errno(EINVAL); // ???
    return -1;
  }

  char addrstr[20];
  sockaddr_in* inaddr = (sockaddr_in*)addr;
  makeaddr4string(addrstr, inaddr);
  err = listener->bind(addrstr, inaddr->sin_port);
  if (listener->error_check(err)) {
    TDLOG(".. dobind error: socket bind failed: %s (%d)",
            socket_strerror(err), err);
    set_errno(EINVAL); // ???
    return -1;
  }
  set_listener(listener);
  return 0;
}


int mbed_socket::dolisten(int backlog) {
  if (backlog > SOMAXCONN) {
    backlog = SOMAXCONN;
  }
  else if (backlog < 1) {
    backlog = 1;
  }

  TCPListener* listener = get_listener();
  if (listener == NULL) {
    TDLOG(".. dolisten error: not binded");
    set_errno(EOPNOTSUPP);
    return -1;
  }

  incomming_ = (void**)malloc(sizeof(void*) * backlog);
  if (incomming_ == NULL) {
    set_errno(ENOMEM);
    return -1;
  }
  incomming_cnt_ = 0;
  incomming_max_ = backlog;
  for (int i=0; i<incomming_max_; i++) {
    incomming_[i] = NULL;
  }

  socket_error_t err;
  err = listener->start_listening(
                      TCPListener::IncomingHandler_t(
                          this, &mbed_socket::on_incoming));
  if (listener->error_check(err)) {
    TDLOG(".. dolisten error: %s (%d)", socket_strerror(err), err);
    return -1;
  }
  return 0;
}


int mbed_socket::doaccept(struct sockaddr *addr, socklen_t *addrlen) {
  void* impl;

  impl = incomming_pop();
  if (impl == NULL) {
    set_errno(EINVAL);
    TDLOG(".. doaccept error: incomming null");
    return -1;
  }

  TCPListener* listener = get_listener();
  if (listener == NULL) {
    set_errno(EOPNOTSUPP);
    TDLOG(".. doaccept error: no listener");
    return -1;
  }

  mbed_socket* socli;
  socli = new mbed_socket(domain_, type_, protocol_);
  if (socli == NULL) {
    set_errno(ENOMEM);
    TDLOG(".. doaccept error: out of memory");
    return -1;
  }
  if (socli->get_sock_fd() < 0) {
    delete socli;
    TDLOG(".. doaccept error: out of fd");
    set_errno(ENOMEM);
    return -1;
  }

  TCPStream* stream;
  stream = listener->accept(impl);
  if (stream == NULL) {
    TDLOG(".. doaccept error: accept failed");
    delete socli;
    set_errno(ENOMEM);
    return -1;
  }

  // it's already connected. there will be no "on_connected" cb called
  socli->set_stream(stream, true);

  stream->setOnError(on_error_t(socli, &mbed_socket::on_error));
  stream->setOnReadable(on_readable_t(socli, &mbed_socket::on_readable));
  stream->setOnSent(on_sent_t(socli, &mbed_socket::on_sent));
  stream->setOnDisconnect(on_disconnect_t(socli, &mbed_socket::on_disconnect));

  //
  if (addr && addrlen) {
    memset(addr, 0, *addrlen);

    struct sockaddr_in* inaddr = (struct sockaddr_in*)addr;

    SocketAddr sockaddr;
    uint16_t sockport;
    stream->getRemoteAddr(&sockaddr);
    stream->getRemotePort(&sockport);
    assert(sockaddr.is_v4());

    inaddr = (struct sockaddr_in*)addr;

    inaddr->sin_family = AF_INET;
    inaddr->sin_addr.s_addr = socket_addr_get_ipv4_addr(sockaddr.getAddr());
    inaddr->sin_port = sockport;

    *addrlen = sizeof(struct sockaddr_in);
  }

  return socli->get_sock_fd();
}


int mbed_socket::doconnect(const struct sockaddr *addr, socklen_t addrlen) {
  (void)addrlen;

  TCPStream* stream;
  stream = new TCPStream(SOCKET_STACK_LWIP_IPV4);
  if (stream == NULL) {
    set_errno(ENOMEM);
    return -1;
  }

  socket_error_t err;
  err = stream->open(SOCKET_AF_INET4);
  if (stream->error_check(err)) {
    TDLOG(".. doconnect error: open failed %s (%d)",
          socket_strerror(err), err);
    set_errno(EBADF); // fix value to connect error no
    return -1;
  }

  set_stream(stream, false);

  sockaddr_in* inaddr = (sockaddr_in*)addr;
  socket_addr s_addr;
  SocketAddr  saddr;

  //char addrstr[20];
  //makeaddr4string(addrstr, inaddr);
  //TDDDLOG(".. connect to %s:%d\r\n", addrstr, inaddr->sin_port);

  socket_addr_set_ipv4_addr(&s_addr, inaddr->sin_addr.s_addr);
  saddr.setAddr(&s_addr);

  stream->setOnError(on_error_t(this, &mbed_socket::on_error));
  stream->setOnReadable(on_readable_t(this, &mbed_socket::on_readable));
  stream->setOnSent(on_sent_t(this, &mbed_socket::on_sent));
  stream->setOnDisconnect(on_disconnect_t(this, &mbed_socket::on_disconnect));

  sock_flag_ |= MBED_SOCKET_FLAG_CONNECTING;
  err = stream->connect(saddr, inaddr->sin_port,
                        on_connect_t(this, &mbed_socket::on_connect));
  if (stream->error_check(err)) {
    TDLOG(".. doconnect error: open failed %s (%d)",
          socket_strerror(err), err);
    set_errno(EBADF); // fix value to connect error no
    return -1;
  }
  set_errno(EINPROGRESS);
  return -1;
}


int mbed_socket::dopoll(struct pollfd* fds) {
  assert(fds->fd == get_sock_fd());

  int ret = 0;

  // 1) check for new connection
  if (get_listener() != NULL && get_incomming() > 0) {
    if (fds->events & POLLIN) {
      fds->revents |= POLLIN;
      ret = 1;
    }
  }
  if (get_stream() != NULL) {
    // 2) check socket disconnection
    if (fds->events & POLLHUP) {
      if (sock_flag_ & MBED_SOCKET_FLAG_DISCONNECTED) {
        fds->revents |= POLLHUP;
        ret = 1;
      }
    }
    // 3) check socket data received
    if (fds->events & POLLIN) {
      if (sock_flag_ & MBED_SOCKET_FLAG_READABLE) {
        fds->revents |= POLLIN;
        ret = 1;
      }
      if (sock_flag_ & MBED_SOCKET_FLAG_DISCONNECTED) {
        fds->revents |= POLLHUP;
        ret = 1;
      }
    }
    // 4) check data send is available
    // and also for when connection is established.
    if (fds->events & POLLOUT) {
      if (sock_flag_ & MBED_SOCKET_FLAG_WRITABLE) {
        fds->revents |= POLLOUT;
        ret = 1;
      }
      if (sock_flag_ & MBED_SOCKET_FLAG_DISCONNECTED) {
        fds->revents |= POLLHUP;
        ret = 1;
      }
      if (sock_flag_ & MBED_SOCKET_FLAG_CONNECTFAILED) {
        fds->revents |= POLLHUP;
        ret = 1;
      }
    }
  }
  //printf("mbed_socket::dopoll fd(%d): %x > %x:%x\r\n",
  //       fds->fd, sock_flag_, fds->events, fds->revents);

  return ret;
}


void mbed_socket::release(void) {
  TCPListener* listener = get_listener();
  if (listener != NULL) {
    delete listener;
    if (incomming_) {
      delete incomming_;
    }
  }
  TCPStream* stream = get_stream();
  if (stream != NULL) {
    delete stream;
  }
  release_slot();
  delete this;
}


void mbed_socket::doclose(void) {
  socket_error_t err;
  TCPListener* listener = get_listener();
  if (listener != NULL) {
    err = listener->stop_listening();
    if (listener->error_check(err)) {
      TDLOG(".. doclose listener 'stop_listening' error: %s (%d)",
            socket_strerror(err), err);
    }
    err = listener->close();
    if (listener->error_check(err)) {
      TDLOG(".. doclose listener 'close' error: %s (%d)",
            socket_strerror(err), err);
    }
  }
  TCPStream* stream = get_stream();
  if (stream != NULL) {
    err = stream->close();
    // it may be already closed by peer. so showing error can be a noise.
    /*
    if (stream->error_check(err)) {
      TDLOG(".. doclose stream 'close' error: %s (%d)",
            socket_strerror(err), err);
    }
    */
  }

  // need to make object destrcution async
  minar::Scheduler::postCallback(mbed::util::FunctionPointer0<void>(this,
                                 &mbed_socket::release).bind())
                    .delay(minar::milliseconds(1))
                    ;
}


int mbed_socket::dogetsockname(struct sockaddr* addr, socklen_t* addrlen) {
  TCPAsynch* async;
  SocketAddr sockaddr;
  socket_error_t err;
  uint16_t sockport = 0;

  async = tcp_.tcp;
  err = async->getLocalAddr(&sockaddr);
  async->getLocalPort(&sockport);
  if (async->error_check(err) || !sockaddr.is_v4()) {
    set_errno(EOPNOTSUPP);
    return -1;
  }

  struct sockaddr_in* inaddr = (struct sockaddr_in*)addr;
  inaddr->sin_family = AF_INET;
  inaddr->sin_addr.s_addr = socket_addr_get_ipv4_addr(sockaddr.getAddr());
  inaddr->sin_port = sockport;

  *addrlen = sizeof(struct sockaddr_in);

  return 0;
}


int mbed_socket::dogetpeername(struct sockaddr* addr, socklen_t* addrlen) {
  TCPAsynch* async;
  SocketAddr sockaddr;
  socket_error_t err;
  uint16_t sockport = 0;

  async = tcp_.tcp;
  err = async->getRemoteAddr(&sockaddr);
  async->getRemotePort(&sockport);
  if (async->error_check(err) || !sockaddr.is_v4()) {
    set_errno(EOPNOTSUPP);
    return -1;
  }

  struct sockaddr_in* inaddr = (struct sockaddr_in*)addr;
  inaddr->sin_family = AF_INET;
  inaddr->sin_addr.s_addr = socket_addr_get_ipv4_addr(sockaddr.getAddr());
  inaddr->sin_port = sockport;

  *addrlen = sizeof(struct sockaddr_in);

  return 0;
}


ssize_t mbed_socket::dowrite(const void* buf, size_t count) {
  TCPStream* stream = get_stream();
  socket_error_t err;

  if ((sock_flag_ & MBED_SOCKET_FLAG_CONNECTED) == 0) {
    TDLOG(".. dowrite, socket not connected");
    set_errno(ENOTCONN);
    return -1;
  }

  sock_flag_ &= ~MBED_SOCKET_FLAG_WRITABLE;

  err = stream->send(buf, count);
  if (stream->error_check(err)) {
    TDLOG(".. socket send error: %s (%d)", socket_strerror(err), err);
    set_errno(EINVAL);
    // TODO: need to check this... how?
    sock_flag_ |= MBED_SOCKET_FLAG_WRITABLE;
    return -1;
  }

  return count;
}


ssize_t mbed_socket::doread(void *buf, size_t count) {
  TCPStream* stream = get_stream();
  socket_error_t err;
  size_t count2;

  if ((sock_flag_ & MBED_SOCKET_FLAG_CONNECTED) == 0) {
    //TDLOG(".. doread, socket not connected");
    set_errno(ENOTCONN);
    // return 0 will let tuv as its EOF
    return 0;
  }

  count2 = count;
  err = stream->recv(buf, &count2);
  if (err != SOCKET_ERROR_NONE) {
    // dont call stream->error_check(err), make it EOF
    TDLOG(".. socket recv error: %s (%d)", socket_strerror(err), err);
    sock_flag_ &= ~MBED_SOCKET_FLAG_READABLE;
    return 0;
  }
  if (count2 < count) {
    // there can be a bug here.
    // when count2 == count, we don't know if there is received
    // packet in mbed socket(lwip?) or not.
    sock_flag_ &= ~MBED_SOCKET_FLAG_READABLE;
  }

  return count2;
}


int mbed_socket::dogetsockopt(int level, int optname, void *optval,
                              socklen_t *optlen) {
  int* intval = (int*)optval;

  switch (level) {
    case SOL_SOCKET :
      switch (optname) {
        case SO_ERROR:
          *intval = get_errno();
          *optlen = sizeof(int);
          return 0;
      }
      break;
  }
  return -1;
}

//-----------------------------------------------------------------------------

void tuvp_tcp_init(void) {
  mbed_socket::init_sockets();

// _eth.init(); // this will make dynamic address
// or set static address
// _eth.init("IP Addrss", "Mask", "Gateway");
#if defined (MBED_IP_ADDRESS)
  _eth.init(MBED_IP_ADDRESS, MBED_IP_MASK, MBED_IP_GATEWAY);
#else
  _eth.init();
#endif
  _eth.connect();
  //TDDDLOG(".. MBED Board IP Address is %s", _eth.getIPAddress());

  lwipv4_socket_init();
}


int tuvp_socket(int domain, int type, int protocol) {
  return mbed_socket::create(domain, type, protocol);
}


uint16_t tuvp_htons(uint16_t hostshort) {
  // TODO fix this
  return hostshort;
}


int tuvp_setsockopt(int sockfd, int level, int optname, const void *optval,
                    socklen_t optlen) {
  // TODO implement this
  (void)sockfd;
  (void)level;
  (void)optname;
  (void)optval;
  (void)optlen;

  // TODO
  // SOL_SOCKET: SO_REUSEADDR, SO_KEEPALIVE
  // IPPROTO_IPV6: IPV6_V6ONLY
  // IPPROTO_TCP: TCP_NODELAY, TCP_KEEPIDLE, TCP_KEEPALIVE
  return 0;
}


int tuvp_getsockopt(int sockfd, int level, int optname, void *optval,
                    socklen_t *optlen) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_bind error: invalid socket(%d)", sockfd);
    return -1;
  }
  return so->dogetsockopt(level, optname, optval, optlen);
}


int tuvp_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL || so->get_listener() != NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_bind error: invalid socket(%d)", sockfd);
    return -1;
  }
  return so->dobind(addr, addrlen);
}


int tuvp_listen(int sockfd, int backlog) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL || so->get_listener() == NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_listen error: invalid socket(%d)", sockfd);
    return -1;
  }
  return so->dolisten(backlog);
}


int tuvp_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL || so->get_listener() == NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_accept error: not a valid socket(%d)", sockfd);
    return -1;
  }
  return so->doaccept(addr, addrlen);
}


int tuvp_connect(int sockfd, const struct sockaddr*addr, socklen_t addrlen) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL || so->get_stream() != NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_connect error: not a valid socket(%d)", sockfd);
    return -1;
  }
  return so->doconnect(addr, addrlen);
}


// emulation for poll, for sockets
int tuvp_net_poll(struct pollfd* fds, int nfds) {
  int i;
  int retcnt = 0;
  mbed_socket* so;

  for (i=0; i<nfds; i++) {
    fds[i].revents = 0;

    so = mbed_socket::fd_to_so(fds[i].fd);
    if (so != NULL) {
      if (so->dopoll(&fds[i])) {
        retcnt++;
      }
    }
  }
  return retcnt;
}


int tuvp_close(int sockfd) {
  mbed_socket* so;

  so = mbed_socket::fd_to_so(sockfd);
  if (so != NULL) {
    if (so->get_listener()) {
      so->doclose();
      return 0;
    }
    else if (so->get_stream()) {
      so->doclose();
      return 0;
    }
    else {
      // socket created but not `bind`ed nor `connect`ed
      delete so;
      return 0;
    }
  }
  return -1;
}


int tuvp_shutdown(int sockfd, int how) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_getsockname error: not a valid socket(%d)", sockfd);
    return -1;
  }
  (void)how;
  // TODO implement this
  return 0;
}


int tuvp_getsockname(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_getsockname error: not a valid socket(%d)", sockfd);
    return -1;
  }
  return so->dogetsockname(addr, addrlen);
}


int tuvp_getpeername(int sockfd, struct sockaddr* addr, socklen_t* addrlen) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_getpeername error: not a valid socket(%d)", sockfd);
    return -1;
  }
  return so->dogetpeername(addr, addrlen);
}


ssize_t tuvp_write(int sockfd, const void* buf, size_t count) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL || so->get_stream() == NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_write error: not a valid socket(%d)", sockfd);
    return -1;
  }
  return so->dowrite(buf, count);
}


ssize_t tuvp_read(int sockfd, void *buf, size_t count) {
  mbed_socket* so = mbed_socket::fd_to_so(sockfd);
  if (so == NULL || so->get_stream() == NULL) {
    set_errno(EBADF);
    TDLOG("tuvp_read error: not a valid socket(%d)", sockfd);
    return -1;
  }
  return so->doread(buf, count);
}
