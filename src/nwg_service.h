#ifndef NWG_SERVICE_H_
#define NWG_SERVICE_H_

#include <map>
#include <memory>
#include <string>

#include <event2/event.h>

#define SZ_1KB      1 << 10
#define SZ_4KB      1 << 12
#define SZ_16KB     1 << 14
#define SZ_32KB     1 << 15
#define SZ_64KB     1 << 16
#define SZ_128KB    1 << 17
#define SZ_256KB    1 << 18
#define SZ_512KB    1 << 19
#define SZ_1MB      1 << 20
#define SZ_2MB      1 << 21
#define SZ_4MB      1 << 22
#define SZ_8MB      1 << 23
#define SZ_16MB     1 << 24
#define SZ_32MB     1 << 25

#define DEFAULT_BUFFSIZE     SZ_32KB
#define DEFAULT_READBUFFSIZE SZ_1MB

#define NWG_ECONNREFUSED 1
#define NWG_EREAD        2
#define NWG_EWRITE       3
#define NWG_ETIMEOUT     4
#define NWG_EUNKNOWN     5

#include "nwg_handler.h"

namespace Nwg
{

class EventLoop;

class ProtocolCodec;

class Service
{
public:
    Service(EventLoop *eventLoop);
    virtual ~Service();

    void setBuffSize(int buffsize);
    void setReadBuffSize(int buffsize);

    void setProtocolCodec(const std::shared_ptr<ProtocolCodec> &protocolCodec);
    void setHandler(const std::shared_ptr<Handler> &handler);

    EventLoop *getEventLoop() const;
    size_t getBuffSize();
    size_t getReadBuffSize();

    Handler &getHandler();
    ProtocolCodec &getProtocolCodec();

protected:
    EventLoop *_eventLoop;

    size_t _buffSize = DEFAULT_BUFFSIZE;
    size_t _readBuffSize = DEFAULT_READBUFFSIZE;

    std::shared_ptr<ProtocolCodec> _protocolCodec;
    std::shared_ptr<Handler> _handler;
};

} /* namespace Nwg */

#endif /* NWG_SERVICE_H_ */
