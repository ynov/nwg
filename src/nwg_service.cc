#include "nwg_service.h"

#include "nwg_eventloop.h"

namespace Nwg
{

Service::Service(EventLoop *eventLoop)
    : _eventLoop(eventLoop),
      _protocolCodec(nullptr),
      _handler(nullptr)
{
}

Service::~Service()
{
    _protocolCodec.reset();
    _handler.reset();
}

void Service::setBuffSize(int buffSize) { _buffSize = buffSize; }
void Service::setReadBuffSize(int readBuffSize) { _readBuffSize = readBuffSize; }

void Service::setProtocolCodec(const std::shared_ptr<ProtocolCodec> &protocolCodec)
{
    _protocolCodec = protocolCodec;
}

void Service::setHandler(const std::shared_ptr<Handler> &handler)
{
    _handler = handler;
}

EventLoop *Service::getEventLoop() const { return _eventLoop; }

size_t Service::getBuffSize() { return _buffSize; }
size_t Service::getReadBuffSize() { return _readBuffSize; }

ProtocolCodec &Service::getProtocolCodec()
{
    return *_protocolCodec;
}

Handler &Service::getHandler()
{
    return *_handler;
}

} /* namespace Nwg */
