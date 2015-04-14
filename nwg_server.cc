#include "nwg_server.h"

namespace Nwg
{

Server::Server(int port)
    : _protocolCodec(nullptr), _handler(nullptr), _port(port)
{
}

Server::~Server()
{
}

std::map<std::string, std::shared_ptr<Object>> &Server::globals()
{
    return _globals;
}

void Server::setProtocolCodec(std::shared_ptr<ProtocolCodec> protocolCodec)
{
    _protocolCodec = protocolCodec;
}

void Server::setHandler(std::shared_ptr<Handler> handler)
{
    _handler = handler;
}

std::shared_ptr<ProtocolCodec> Server::protocolCodec()
{
    return _protocolCodec;
}


std::shared_ptr<Handler> Server::handler()
{
    return _handler;
}

int Server::port()
{
    return _port;
}

void Server::setPort(int port)
{
    _port = port;
}

void Server::evcb_doAccept(evutil_socket_t listener, short event, void *arg)
{
    printf("doAccept()\n");

    ListenerEventArg *listenerEventArg = (ListenerEventArg *) arg;
    struct event_base *base = listenerEventArg->base;
    Server *server = listenerEventArg->server;

    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr *) &ss, &slen);
    if (fd < 0) {
        perror("accept()");
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        evutil_make_socket_nonblocking(fd);

        // std::shared_ptr<Session> session(new Session(4096, base, fd, server));
        Session *session = new Session(4096, base, fd, server);

        server->handler()->sessionOpened(*session);
        event_add(session->readEvent, NULL);
    }
}

void Server::evcb_doRead(evutil_socket_t fd, short events, void *arg)
{
    printf("doRead()\n");
    Session *session = (Session *) arg;

    char buf[1024];
    ssize_t result;

    while (1) {
        result = recv(fd, buf, sizeof(buf), 0);
        if (result <= 0)
            break;

        session->getReadBuffer().putBytes(buf, result);
    }

    session->getReadBuffer().flip();
    Nwg::ObjectContainer oc;

    session->server()->protocolCodec()->encode(session->getReadBuffer(), oc);
    session->server()->handler()->messageReceived(*session, *oc.object());

    event_add(session->writeEvent, NULL);
    // session->close();
    // delete session;
}

void Server::evcb_doWrite(evutil_socket_t fd, short events, void *arg)
{
    printf("doWrite()\n");
    Session *session = (Session *) arg;

    session->server()->protocolCodec()->decode(session->getWriteObject(), session->getWriteBuffer());
    std::vector<byte> b =  session->getWriteBuffer().getBytes(session->getWriteBuffer().remaining());
    ssize_t result = send(fd, b.data(), b.size(), 0);

    event_del(session->writeEvent);

    session->server()->handler()->messageSent(*session, session->getWriteObject());

    if (session->closed) {
        delete session;
    }
}

void Server::run()
{
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

    do {
        int one = 1;
        setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    } while(0);

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

    _listenerEvent = event_new(base, _listener, EV_READ | EV_PERSIST,
        evcb_doAccept, (void *) &listenerEventArg);

    event_add(_listenerEvent, NULL);

    event_base_dispatch(base);
}

} /* namespace Nwg */
