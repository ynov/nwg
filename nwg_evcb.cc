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

#ifdef _WIN32
#define close closesocket
#endif

#ifndef SILENT
#define SILENT 1
#endif

#if SILENT
#define _dprintf(...)
#else
#define _dprintf(...) fprintf (stdout, __VA_ARGS__)
#endif

namespace Nwg
{

void EVCB::doAccept(evutil_socket_t listener, short event, void *arg)
{
    _dprintf("D-- %40s --\n", "doAccept() IN");

    ListenerEventArg &listenerEventArg = *(ListenerEventArg *) arg;

    struct event_base *base = listenerEventArg.base;
    Server &server          = *listenerEventArg.server;
    Handler &handler        = server.getHandler();

    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr *) &ss, &slen);
    if (fd < 0) {
        perror("accept()");
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        evutil_make_socket_nonblocking(fd);

        Session *session = new Session(server.getBuffSize(), base, fd, &server);
        session->resetWrite();

        handler.sessionOpened(*session);

        if (session->isClosed()) {
            close(fd);

            handler.sessionClosed(*session);

            _dprintf("D-- %40s --\n", "doAccept() OUT (closed)");
            delete session;
            return;
        }

        if (session->isWriteObjectPresent()) {
            event_add(session->writeEvent, NULL);
        } else {
            event_add(session->readEvent, NULL);
        }
    }

    _dprintf("D-- %40s --\n", "doAccept() OUT");
}

void EVCB::doRead(evutil_socket_t fd, short events, void *arg)
{
    _dprintf("D-- %40s --\n", "doRead() IN");

    Session *session = (Session *) arg;

    Server &server               = session->getServer();
    ProtocolCodec &protocolCodec = server.getProtocolCodec();
    Handler &handler             = server.getHandler();
    ByteBuffer &readBuffer       = session->getReadBuffer();
    ByteBuffer &writeBuffer      = session->getWriteBuffer();

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
        handler.sessionClosed(*session);

        _dprintf("D-- %40s --\n", "doRead() OUT (closed, result == 0)");
        delete session;
        return;
    } else if (result < 0) {
        bool err_eagain = false;

#ifdef __unix__
        err_eagain = errno == EAGAIN;
#endif /* __unix__ */

#ifdef _WIN32
        err_eagain = WSAGetLastError() == WSAEWOULDBLOCK;
        errno = WSAGetLastError();
#endif /* _W32 */

        if (!err_eagain) {
            handler.sessionClosed(*session);

            perror("recv()");

            _dprintf("D-- %40s --\n", "doRead() OUT (closed, err_not_eagain)");
            _dprintf("DDDD-- errno == %d\n", errno);
            delete session;
            return;
        }
    }

    Nwg::ObjectContainer oc;
    protocolCodec.transformUp(&session->getReadBuffer(), &oc);

    session->resetWrite();
    handler.messageReceived(*session, oc.getObject());

    if (session->isClosed()) {
        event_del(session->readEvent);

        close(fd);

        handler.sessionClosed(*session);

        _dprintf("D-- %40s --\n", "doRead() OUT (closed)");
        delete session;
        return;
    }

    if (session->isWriteObjectPresent()) {
        event_add(session->writeEvent, NULL);
    }

    _dprintf("D-- %40s --\n", "doRead() OUT");
}

void EVCB::doWrite(evutil_socket_t fd, short events, void *arg)
{
    _dprintf("D-- %40s --\n", "doWrite() IN");

    Session *session = (Session *) arg;

    Server &server               = session->getServer();
    ProtocolCodec &protocolCodec = server.getProtocolCodec();
    Handler &handler             = server.getHandler();
    ByteBuffer &writeBuffer      = session->getWriteBuffer();

    if (session->nWritten == 0) {
        protocolCodec.transformDown(&session->getWriteObject(), &writeBuffer);
    }

    while (session->nWritten < writeBuffer.limit()) {
        std::vector<byte> b = writeBuffer.read(writeBuffer.remaining());
        ssize_t result = send(fd, (char *) b.data(), b.size(), 0);

        // _dprintf("DDDD-- writeBuffer.limit() = %d --\n", (int) writeBuffer.limit());
        // _dprintf("DDDD-- b.size() = %d --\n", (int) b.size());
        // _dprintf("DDDD-- result = %d --\n", (int) result);

        if (result < 0) {
            bool err_eagain = false;

#ifdef __unix__
            err_eagain = errno == EAGAIN;
#endif /* __unix__ */

#ifdef _WIN32
            err_eagain = WSAGetLastError() == WSAEWOULDBLOCK;
            errno = WSAGetLastError();
#endif /* _W32 */

            if (err_eagain) {
                _dprintf("DDDD-- errno == EAGAIN (OK)\n");
                return;
            } else {
                _dprintf("DDDD-- errno IS NOT EAGAIN\n");
                return;
            }
        }

        // _dprintf("DDDD-- session.nWritten (%d) += result (%d) --\n", session.nWritten, result);
        session->nWritten += result;
        // _dprintf("DDDD-- session.nWritten (%d) | limit() = %d --\n", session.nWritten, writeBuffer.limit());
        writeBuffer.jump(session->nWritten);
    }

    if (session->nWritten != writeBuffer.limit()) {
        _dprintf("DDDD-- it suddenly goes here...\n");
        _dprintf("DDDD-- session.nWritten == %d, writeBuffer.limit() == %d\n", session.nWritten, writeBuffer.limit());
        return;
    }

    session->nWritten = 0;

    session->resetWrite();
    handler.messageSent(*session, session->getLastWriteObject());

    if (session->isClosed()) {
        event_del(session->writeEvent);

        close(fd);

        handler.sessionClosed(*session);

        _dprintf("D-- %40s --\n", "doWrite() OUT (closed)");
        delete session;
        return;
    }

    if (!session->isWriteObjectPresent()) {
        event_del(session->writeEvent);
        event_add(session->readEvent, NULL);
    }

    _dprintf("D-- %40s --\n", "doWrite() OUT");
}

} /* namespace Nwg */

