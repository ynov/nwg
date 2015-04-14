#ifndef NWG_SERVER_H_
#define NWG_SERVER_H_

#include <map>
#include <memory>

#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

#include <event2/event.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include "nwg_object.h"
#include "nwg_handler.h"
#include "nwg_protocolcodec.h"

namespace Nwg
{

class Server : public Object
{
public:
    Server(int port);
    virtual ~Server();

    std::map<std::string, std::shared_ptr<Object>> &globals();
    void setProtocolCodec(std::shared_ptr<ProtocolCodec> _protocolCodec);
    void setHandler(std::shared_ptr<Handler> _handler);

    std::shared_ptr<Handler> handler();
    std::shared_ptr<ProtocolCodec> protocolCodec();

    void setPort(int port);
    int port();
    void run();

    static void evcb_doRead(evutil_socket_t fd, short events, void *arg);
    static void evcb_doWrite(evutil_socket_t fd, short events, void *arg);

private:
    static void evcb_doAccept(evutil_socket_t listener, short event, void *arg);

private:
    std::map<std::string, std::shared_ptr<Object>> _globals;
    std::shared_ptr<ProtocolCodec> _protocolCodec;
    std::shared_ptr<Handler> _handler;

    int _port;
    evutil_socket_t _listener = -1;
    struct event *_listenerEvent = nullptr;
};

struct ListenerEventArg {
    struct event_base *base;
    Server *server;
};

} /* namespace Nwg */

#endif /* NWG_SERVER_H_ */
