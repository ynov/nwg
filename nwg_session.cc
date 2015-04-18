#include "nwg_session.h"

#include "nwg_server.h"
#include "nwg_evcb.h"

namespace Nwg
{

Session::Session(int bufferAllocationSize,
        struct event_base *base,
        evutil_socket_t fd,
        Server *server)
    : _bufferAllocationSize(bufferAllocationSize),
      _writeObject(nullptr),
      _lastWriteObject(nullptr),
      _readBuffer(ByteBuffer(bufferAllocationSize)),
      _writeBuffer(ByteBuffer(bufferAllocationSize)),
      _fd(fd),
      _server(server)
{
    if (base == nullptr) {
        return;
    }

    readEvent = event_new(base, _fd,
            EV_READ | EV_PERSIST,
            EVCB::doRead,
            (void *) this);
    writeEvent = event_new(base, _fd,
            EV_WRITE | EV_PERSIST,
            EVCB::doWrite,
            (void *) this);
}

Session::~Session()
{
    if (readEvent != nullptr)
        event_free(readEvent);
    if (writeEvent != nullptr)
        event_free(writeEvent);

    for (auto &_ : _vars) {
        _.second.reset();
    }
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

bool Session::isWriteObjectPresent()
{
    return _writeObject.get() != nullptr;
}

void Session::resetWriteObject()
{
    if (_writeObject.get() != nullptr) {
        _lastWriteObject = std::move(_writeObject);
        _writeObject.reset();
        _writeObject = nullptr;
    }
}

void Session::resetWrite()
{
    resetWriteObject();
    _writeBuffer.reset();
}

Object &Session::getWriteObject()
{
    return *_writeObject;
}

Object &Session::getLastWriteObject()
{
    return *_lastWriteObject;
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
