#ifndef NWG_SERVER_H_
#define NWG_SERVER_H_

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

#include "nwg_handler.h"

namespace Nwg
{

class ProtocolCodec;

class Server : public Object
{
public:
    Server(int port);
    virtual ~Server();

    void setProtocolCodec(const std::shared_ptr<ProtocolCodec> &protocolCodec);
    void setHandler(const std::shared_ptr<Handler> &handler);

    Handler &getHandler();
    ProtocolCodec &getProtocolCodec();

    void setPort(int port);
    void setBuffSize(int buffsize);
    void setReadBuffSize(int buffsize);

    int getPort();
    size_t getBuffSize();
    size_t getReadBuffSize();

    void run();

private:
    std::shared_ptr<ProtocolCodec> _protocolCodec;
    std::shared_ptr<Handler> _handler;

    int _port;
    evutil_socket_t _listener = -1;
    struct event *_listenerEvent = nullptr;
    size_t _buffSize = DEFAULT_BUFFSIZE;
    size_t _readBuffSize = DEFAULT_READBUFFSIZE;
};

struct ListenerEventArg {
    struct event_base *base;
    Server *server;
};

} /* namespace Nwg */

#endif /* NWG_SERVER_H_ */
