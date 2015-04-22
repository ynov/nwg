#include "nwg_common_socket_include.h"

#include "nwg_server.h"
#include "nwg_evcb.h"

#include "nwg_object.h"
#include "nwg_handler.h"
#include "nwg_protocolcodec.h"

namespace Nwg
{

Server::Server(int port)
    : _protocolCodec(nullptr), _handler(nullptr), _port(port)
{
}

Server::~Server()
{
    _protocolCodec.reset();
    _handler.reset();
}

void Server::setProtocolCodec(const std::shared_ptr<ProtocolCodec> &protocolCodec)
{
    _protocolCodec = protocolCodec;
}

void Server::setHandler(const std::shared_ptr<Handler> &handler)
{
    _handler = handler;
}

ProtocolCodec &Server::getProtocolCodec()
{
    return *_protocolCodec;
}

Handler &Server::getHandler()
{
    return *_handler;
}

int Server::getPort()
{
    return _port;
}

int Server::getBuffSize()
{
    return _buffSize;
}

int Server::getReadBuffSize()
{
    return _readBuffSize;
}

void Server::setPort(int port)
{
    _port = port;
}

void Server::setBuffSize(int buffSize)
{
    _buffSize = buffSize;
}

void Server::setReadBuffSize(int readBuffSize)
{
    _readBuffSize = readBuffSize;
}

void Server::run()
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
    struct event_base *base = event_base_new();

    if (!base) {
        perror("event_base_new()");
        return;
    }

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(_port);

    _listener = socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(_listener);

#ifdef __unix__
    do {
        int one = 1;
        setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    } while(0);
#endif /* __unix__ */

    if (bind(_listener, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("bind()");
        return;
    }

    if (listen(_listener, 16) < 0) {
        perror("listen()");
        return;
    }

    ListenerEventArg listenerEventArg;
    listenerEventArg.base = base;
    listenerEventArg.server = this;

    _listenerEvent = event_new(base, _listener,
            EV_READ | EV_PERSIST,
            EVCB::doAccept,
            (void *) &listenerEventArg);

    event_add(_listenerEvent, NULL);

    event_base_dispatch(base);
}

} /* namespace Nwg */
