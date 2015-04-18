#include "nwg_evcb.h"

#include "nwg_common_socket_include.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include "nwg_object.h"
#include "nwg_objectcontainer.h"
#include "nwg_bytebuffer.h"
#include "nwg_protocolcodec.h"
#include "nwg_handler.h"
#include "nwg_server.h"
#include "nwg_session.h"

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

        readBuffer.put(buf, result);
    }
    readBuffer.flip();

    if (result == 0) {
        handler.sessionClosed(session);
        delete &session;

        return;
    } else if (result < 0) {
        bool err_not_eagain = false;

#ifdef __unix__
        err_not_eagain = errno != EAGAIN;
#endif /* __unix__ */

#ifdef _WIN32
        err_not_eagain = WSAGetLastError() != WSAEWOULDBLOCK;
#endif /* _W32 */

        if (err_not_eagain) {
            handler.sessionClosed(session);
            delete &session;

            perror("recv()");

            return;
        }
    }

    Nwg::ObjectContainer oc;
    protocolCodec.encode(&session.getReadBuffer(), &oc);

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

    protocolCodec.decode(&session.getWriteObject(), &writeBuffer);

    std::vector<byte> b = writeBuffer.read(writeBuffer.remaining());
    ssize_t result = send(fd, (char *) b.data(), b.size(), 0);

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

} /* namespace Nwg */

