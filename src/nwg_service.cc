#include "nwg_service.h"

namespace Nwg
{

Service::Service(const Service *service)
    : _base(nullptr),
      _protocolCodec(nullptr),
      _handler(nullptr)
{
    if (service != nullptr) {
        _base = service->getBase();
    }

    if (_base == nullptr) {
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

        _base = event_base_new();
        if (!_base) {
            perror("event_base_new()");
            return;
        }
    }
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

struct event_base *Service::getBase() const { return _base; }

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
