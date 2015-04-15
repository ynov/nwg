#include "nwg_session.h"

#include "nwg_server.h"

namespace Nwg
{

Session::Session(int bufferAllocationSize,
        struct event_base *base,
        evutil_socket_t fd,
        Server *server)
    : _bufferAllocationSize(bufferAllocationSize),
      _writeObject(nullptr),
      _readBuffer(ByteBuffer(bufferAllocationSize)),
      _writeBuffer(ByteBuffer(bufferAllocationSize)),
      _fd(fd),
      _server(server)
{
    if (base == NULL) {
        return;
    }

    readEvent = event_new(base, _fd, EV_READ | EV_PERSIST,
            Server::evcb_doRead, (void *) this);
    writeEvent = event_new(base, _fd, EV_WRITE | EV_PERSIST,
            Server::evcb_doWrite, (void *) this);
}

Session::~Session()
{
    if (readEvent != nullptr)
        event_free(readEvent);
    if (writeEvent != nullptr)
        event_free(writeEvent);
}

void Session::write(std::shared_ptr<Object> obj)
{
    _writeObject = obj;
}

void Session::write(Object *obj)
{
    _writeObject = std::shared_ptr<Object>(obj);
}

void Session::close()
{
    _closed = true;
}

int Session::getBufferAllocationSize()
{
    return _bufferAllocationSize;
}

Object &Session::getWriteObject()
{
    return *_writeObject;
}

ByteBuffer &Session::getReadBuffer()
{
    return _readBuffer;
}

ByteBuffer &Session::getWriteBuffer()
{
    return _writeBuffer;
}

Server &Session::getServer()
{
    return *_server;
}

bool Session::isClosed()
{
    return _closed;
}

int Session::getFd()
{
    return _fd;
}

} /* namespace Nwg */
