#include "nwg_server.h"

namespace Nwg
{

void EVCB::doAccept(evutil_socket_t listener, short event, void *arg)
{
    // printf("doAccept()\n");

    ListenerEventArg &listenerEventArg = *(ListenerEventArg *) arg;

    struct event_base *base = listenerEventArg.base;
    Server &server = *listenerEventArg.server;
    Handler &handler = server.getHandler();

    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr *) &ss, &slen);
    if (fd < 0) {
        perror("accept()");
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        evutil_make_socket_nonblocking(fd);

        Session &session = *new Session(server.getBuffSize(), base, fd, &server);
        session.resetWrite();

        handler.sessionOpened(session);

        if (session.isClosed()) {
            close(fd);
            handler.sessionClosed(session);
            delete &session;

            return;
        }

        if (session.isWriteObjectPresent()) {
            event_add(session.writeEvent, NULL);
        } else {
            event_add(session.readEvent, NULL);
        }
    }
}

void EVCB::doRead(evutil_socket_t fd, short events, void *arg)
{
    // printf("doRead()\n");

    Session &session = *((Session *) arg);
    Server &server = session.getServer();
    ProtocolCodec &protocolCodec = server.getProtocolCodec();
    Handler &handler = server.getHandler();
    ByteBuffer &readBuffer = session.getReadBuffer();
    ByteBuffer &writeBuffer = session.getWriteBuffer();

    char buf[SMALL_BUFFSIZE];
    ssize_t result;

    while (1) {
        result = recv(fd, buf, sizeof(buf), 0);
        if (result <= 0)
            break;

        readBuffer.putBytes(buf, result);
    }
    readBuffer.flip();

    if (result == 0) {
        handler.sessionClosed(session);
        delete &session;

        return;
    } else if (result < 0) {
        if (errno != EAGAIN) {
            handler.sessionClosed(session);
            delete &session;

            perror("recv()");

            return;
        }
    }

    Nwg::ObjectContainer oc;
    protocolCodec.encode(session.getReadBuffer(), oc);

    session.resetWrite();
    handler.messageReceived(session, oc.getObject());

    if (session.isClosed()) {
        event_del(session.readEvent);

        close(fd);
        handler.sessionClosed(session);
        delete &session;

        return;
    }

    if (session.isWriteObjectPresent()) {
        event_add(session.writeEvent, NULL);
    }
}

void EVCB::doWrite(evutil_socket_t fd, short events, void *arg)
{
    // printf("doWrite()\n");

    Session &session = *((Session *) arg);
    Server &server = session.getServer();
    ProtocolCodec &protocolCodec = server.getProtocolCodec();
    Handler &handler = server.getHandler();
    ByteBuffer &writeBuffer = session.getWriteBuffer();

    protocolCodec.decode(session.getWriteObject(), writeBuffer);

    std::vector<byte> b = writeBuffer.getBytes(writeBuffer.remaining());
    ssize_t result = send(fd, b.data(), b.size(), 0);

    if (result < 0) {
        // TODO
    }

    session.resetWrite();
    handler.messageSent(session, session.getWriteObject());

    if (session.isClosed()) {
        event_del(session.writeEvent);

        close(fd);
        handler.sessionClosed(session);
        delete &session;

        return;
    }

    if (!session.isWriteObjectPresent()) {
        event_del(session.writeEvent);
        event_add(session.readEvent, NULL);
    }
}


Server::Server(int port)
    : _protocolCodec(nullptr), _handler(nullptr), _port(port)
{
}

Server::~Server()
{
    _protocolCodec.reset();
    _handler.reset();
}

void Server::setProtocolCodec(ProtocolCodec *protocolCodec)
{
    _protocolCodec = std::shared_ptr<ProtocolCodec>(protocolCodec);
}

void Server::setHandler(Handler *handler)
{
    _handler = std::shared_ptr<Handler>(handler);
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

void Server::setPort(int port)
{
    _port = port;
}

void Server::setBuffSize(int buffSize)
{
    _buffSize = buffSize;
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

    _listenerEvent = event_new(base, _listener,
            EV_READ | EV_PERSIST,
            EVCB::doAccept,
            (void *) &listenerEventArg);

    event_add(_listenerEvent, NULL);

    event_base_dispatch(base);
}

} /* namespace Nwg */
