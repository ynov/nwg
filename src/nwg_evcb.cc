#include "nwg_evcb.h"

#include "nwg_common_socket_include.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include "nwg_acceptor.h"
#include "nwg_connector.h"
#include "nwg_object.h"
#include "nwg_objectcontainer.h"
#include "nwg_bytebuffer.h"
#include "nwg_protocolcodec.h"
#include "nwg_handler.h"
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
    Acceptor &acceptor      = *listenerEventArg.acceptor;
    Handler &handler        = acceptor.getHandler();

    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

    int fd = accept(listener, (struct sockaddr *) &ss, &slen);
    if (fd < 0) {
        perror("accept()");
    } else if (fd > FD_SETSIZE) {
        close(fd);
    } else {
        evutil_make_socket_nonblocking(fd);

        Session *session = new Session(acceptor.getBuffSize(), base, fd, &acceptor);
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

void EVCB::doConnect(evutil_socket_t fd, short event, void *arg)
{
    _dprintf("D-- %40s --\n", "doConnect() IN");

    ConnectorEventArg &connectorEventArg = *(ConnectorEventArg *) arg;

    struct event_base *base = connectorEventArg.base;
    Connector &connector    = *connectorEventArg.connector;
    Handler &handler        = connector.getHandler();

    char buff[16];
    size_t result = 0;

    while (1) {
        result = recv(fd, buff, 0, 0);
        if (result <= 0) {
            break;
        }
    }

    do {
        bool err_conn_refused = false;

#ifdef __unix__
        err_conn_refused = errno == ECONNREFUSED;
#endif /* __unix__ */

#ifdef _WIN32
        errno = WSAGetLastError();
        err_conn_refused = errno == WSAECONNREFUSED;
#endif /* _WIN32 */

        if (err_conn_refused) {
            perror("read()");
            return;
        }
    } while(0);

    Session *session = new Session(connector.getBuffSize(), base, fd, &connector);
    session->resetWrite();

    handler.sessionOpened(*session);

    if (session->isClosed()) {
        close(fd);

        handler.sessionClosed(*session);

        delete session;
        return;
    }

    if (session->isWriteObjectPresent()) {
        event_add(session->writeEvent, NULL);
    } else {
        event_add(session->readEvent, NULL);
    }

    _dprintf("D-- %40s --\n", "doConnect() OUT");
}

void EVCB::doRead(evutil_socket_t fd, short events, void *arg)
{
    _dprintf("D-- %40s --\n", "doRead() IN");

    Session *session = (Session *) arg;

    Service &service             = session->getService();
    ProtocolCodec &protocolCodec = service.getProtocolCodec();
    Handler &handler             = service.getHandler();
    ByteBuffer &readBuffer       = session->getReadBuffer();
    ByteBuffer &writeBuffer      = session->getWriteBuffer();
    size_t readBuffSize          = service.getReadBuffSize();

    char *buff = new char[readBuffSize];
    ssize_t result = 0;

    while (1 && session->nRead < readBuffSize) {
        result = recv(fd, buff, readBuffSize, 0);
        if (result <= 0) {
            break;
        }

        readBuffer.put(buff, result);
        session->nRead += result;
    }
    readBuffer.flip();

    delete [] buff;

    if (session->nRead >= readBuffSize) {
        session->stillReading = true;
    }

    if (result == 0) {
        handler.sessionClosed(*session);

        _dprintf("D-- %40s --\n", "doRead() OUT (closed, result == 0)");
        delete session;
        return;
    } else if (result < 0) {
        session->nRead = 0;
        session->stillReading = false;

        bool err_eagain = false;

#ifdef __unix__
        err_eagain = (errno == EAGAIN || errno == EWOULDBLOCK);
#endif /* __unix__ */

#ifdef _WIN32
        err_eagain = WSAGetLastError() == WSAEWOULDBLOCK;
        errno = WSAGetLastError();
#endif /* _W32 */

        if (!err_eagain) {
            // TODO: throw error
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
        if (session->stillReading) {
            // TODO: throw error
            return;
        }

        event_add(session->writeEvent, NULL);
    }

    _dprintf("D-- %40s --\n", "doRead() OUT");
}

void EVCB::doWrite(evutil_socket_t fd, short events, void *arg)
{
    _dprintf("D-- %40s --\n", "doWrite() IN");

    Session *session = (Session *) arg;

    Service &service             = session->getService();
    ProtocolCodec &protocolCodec = service.getProtocolCodec();
    Handler &handler             = service.getHandler();
    ByteBuffer &writeBuffer      = session->getWriteBuffer();

    if (session->nWritten == 0) {
        protocolCodec.transformDown(&session->getWriteObject(), &writeBuffer);
    }

    while (session->nWritten < writeBuffer.limit()) {
        std::vector<byte> bs;

        writeBuffer.read(bs, writeBuffer.remaining());
        ssize_t result = send(fd, (char *) bs.data(), bs.size(), 0);

        if (result < 0) {
            bool err_eagain = false;

#ifdef __unix__
            err_eagain = (errno == EAGAIN || errno == EWOULDBLOCK);
#endif /* __unix__ */

#ifdef _WIN32
            err_eagain = WSAGetLastError() == WSAEWOULDBLOCK;
            errno = WSAGetLastError();
#endif /* _W32 */

            if (err_eagain) {
                _dprintf("DDDD-- errno == EAGAIN (OK)\n");
                return;
            } else {
                // TODO: throw error
                _dprintf("DDDD-- errno IS NOT EAGAIN, errno = %d\n", errno);
                return;
            }
        }

        session->nWritten += result;
        writeBuffer.jump(session->nWritten);
    }

    if (session->nWritten != writeBuffer.limit()) {
        // TODO: throw error
        _dprintf("DDDD-- it suddenly goes here...\n");
        _dprintf("DDDD-- session.nWritten == %d, writeBuffer.limit() == %d\n", (int) session->nWritten, (int) writeBuffer.limit());
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

