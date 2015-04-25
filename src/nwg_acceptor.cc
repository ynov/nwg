#include "nwg_common_socket_include.h"

#include <cstring>

#include "nwg_acceptor.h"
#include "nwg_eventloop.h"

#include "nwg_evcb.h"

namespace Nwg
{

Acceptor::Acceptor(int port, EventLoop *eventLoop)
    : Service(eventLoop),
      _port(port),
      _listenerFd(-1),
      _listenerEvent(nullptr),
      _listenerEventArg(nullptr)
{
}

Acceptor::~Acceptor()
{
    if (_listenerEventArg != nullptr) {
        delete _listenerEventArg;
    }

    if (_listenerEvent != nullptr) {
        event_del(_listenerEvent);
    }

    ::close(_listenerFd);
}

void Acceptor::setPort(int port) { _port = port; }
int Acceptor::getPort() { return _port; }

void Acceptor::listen()
{
    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port        = htons(_port);

    _listenerFd = ::socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(_listenerFd);

#ifdef __unix__
    do {
        int one = 1;
        ::setsockopt(_listenerFd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    } while(0);
#endif /* __unix__ */

    if (::bind(_listenerFd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("bind()");
        return;
    }

    if (::listen(_listenerFd, 16) < 0) {
        perror("listen()");
        return;
    }

    struct event_base *base = _eventLoop->getBase();

    _listenerEventArg = new ListenerEventArg();
    _listenerEventArg->base     = base;
    _listenerEventArg->acceptor = this;

    _listenerEvent = event_new(base, _listenerFd,
            EV_READ | EV_PERSIST,
            EVCB::doAccept,
            (void *) _listenerEventArg);

    event_add(_listenerEvent, NULL);
}

} /* namespace Nwg */
