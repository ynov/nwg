#include "nwg_common_socket_include.h"

#include <cstring>

#include "nwg_acceptor.h"
#include "nwg_evcb.h"

#include "nwg_object.h"
#include "nwg_handler.h"
#include "nwg_protocolcodec.h"

namespace Nwg
{

Acceptor::Acceptor(int port)
    : _base(nullptr),
      _protocolCodec(nullptr),
      _handler(nullptr),
      _port(port)
{
}

Acceptor::~Acceptor()
{
    _protocolCodec.reset();
    _handler.reset();
}

void Acceptor::setProtocolCodec(const std::shared_ptr<ProtocolCodec> &protocolCodec)
{
    _protocolCodec = protocolCodec;
}

void Acceptor::setHandler(const std::shared_ptr<Handler> &handler)
{
    _handler = handler;
}

ProtocolCodec &Acceptor::getProtocolCodec()
{
    return *_protocolCodec;
}

Handler &Acceptor::getHandler()
{
    return *_handler;
}

int Acceptor::getPort()
{
    return _port;
}

size_t Acceptor::getBuffSize()
{
    return _buffSize;
}

size_t Acceptor::getReadBuffSize()
{
    return _readBuffSize;
}

void Acceptor::setPort(int port)
{
    _port = port;
}

void Acceptor::setBuffSize(int buffSize)
{
    _buffSize = buffSize;
}

void Acceptor::setReadBuffSize(int readBuffSize)
{
    _readBuffSize = readBuffSize;
}

struct event_base *Acceptor::getBase()
{
    return _base;
}

void Acceptor::listen()
{
#ifdef _WIN32
    do {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD(2, 2);

        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0) {
            printf("WSAStartup failed with error: %d\n", err);
            return;
        }
    } while(0);
#endif /* _WIN32 */

    struct sockaddr_in sin;

    _base = event_base_new();
    if (!_base) {
        perror("event_base_new()");
        return;
    }

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

    ListenerEventArg *listenerEventArg = new ListenerEventArg();
    listenerEventArg->base   = _base;
    listenerEventArg->acceptor = this;

    _listenerEvent = event_new(_base, _listenerFd,
            EV_READ | EV_PERSIST,
            EVCB::doAccept,
            (void *) listenerEventArg);

    event_add(_listenerEvent, NULL);

    event_base_dispatch(_base);

    delete listenerEventArg;
}

} /* namespace Nwg */
