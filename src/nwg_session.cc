#include "nwg_session.h"

#include "nwg_service.h"
#include "nwg_evcb.h"

namespace Nwg
{

Session::Session(int bufferAllocationSize,
        struct event_base *base,
        evutil_socket_t fd,
        Service *service)
    : _bufferAllocationSize(bufferAllocationSize),
      _writeObject(nullptr),
      _lastWriteObject(nullptr),
      _readBuffer(MessageBuffer(bufferAllocationSize)),
      _writeBuffer(MessageBuffer(bufferAllocationSize)),
      _fd(fd),
      _service(service)
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
    if (readEvent != nullptr) {
        event_free(readEvent);
    }

    if (writeEvent != nullptr) {
        event_free(writeEvent);
    }

    for (auto &_ : _vars) {
        _.second.reset();
    }
}

void Session::write(const std::shared_ptr<MessageBuffer> &obj)
{
    _writeObject = obj;
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

MessageBuffer &Session::getWriteObject()
{
    return *_writeObject;
}

MessageBuffer &Session::getLastWriteObject()
{
    return *_lastWriteObject;
}

MessageBuffer &Session::getReadBuffer()
{
    return _readBuffer;
}

MessageBuffer &Session::getWriteBuffer()
{
    return _writeBuffer;
}

Service &Session::getService()
{
    return *_service;
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
