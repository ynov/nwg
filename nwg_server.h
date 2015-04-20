#ifndef NWG_SERVER_H_
#define NWG_SERVER_H_

#include <map>
#include <memory>
#include <string>

#include <event2/event.h>

#define DEFAULT_BUFFSIZE 32768
#define SMALL_BUFFSIZE 1024

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

    int getPort();
    int getBuffSize();

    void run();

private:
    std::shared_ptr<ProtocolCodec> _protocolCodec;
    std::shared_ptr<Handler> _handler;

    int _port;
    evutil_socket_t _listener = -1;
    struct event *_listenerEvent = nullptr;
    int _buffSize = DEFAULT_BUFFSIZE;
};

struct ListenerEventArg {
    struct event_base *base;
    Server *server;
};

} /* namespace Nwg */

#endif /* NWG_SERVER_H_ */
