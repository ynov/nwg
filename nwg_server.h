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

#define DEFAULT_BUFFSIZE 32768
#define SMALL_BUFFSIZE 1024

namespace Nwg
{

struct EVCB
{
    static void doAccept(evutil_socket_t listener, short event, void *arg);
    static void doRead(evutil_socket_t fd, short events, void *arg);
    static void doWrite(evutil_socket_t fd, short events, void *arg);
};

class Server : public Object
{
public:
    Server(int port);
    virtual ~Server();

    void setProtocolCodec(ProtocolCodec *_protocolCodec);
    void setHandler(Handler *_handler);

    Handler &getHandler();
    ProtocolCodec &getProtocolCodec();

    void setPort(int port);
    void setBuffSize(int buffsize);

    int getPort();
    int getBuffSize();

    void run();

    template<class T>
    void put(const std::string &key, std::shared_ptr<T> val)
    {
        _vars[key] = val;
    }

    template<class T>
    void put(const std::string &key, T *val)
    {
        _vars[key] = std::shared_ptr<T>(val);
    }

    template<class T>
    T &get(const std::string &key)
    {
        return *(T *) _vars[key].get();
    }

private:
    std::shared_ptr<ProtocolCodec> _protocolCodec;
    std::shared_ptr<Handler> _handler;

    int _port;
    evutil_socket_t _listener = -1;
    struct event *_listenerEvent = nullptr;
    int _buffSize = DEFAULT_BUFFSIZE;

    std::map<std::string, std::shared_ptr<void>> _vars;
};

struct ListenerEventArg {
    struct event_base *base;
    Server *server;
};

} /* namespace Nwg */

#endif /* NWG_SERVER_H_ */
