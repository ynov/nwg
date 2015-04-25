#ifndef NWG_ACCEPTOR_H_
#define NWG_ACCEPTOR_H_

#include "nwg_service.h"

namespace Nwg
{

struct ListenerEventArg;

class EventLoop;

class Acceptor : public Service
{
public:
    Acceptor(EventLoop *eventLoop, int port = 1024);
    virtual ~Acceptor();

    void setPort(int port);
    int getPort();

    void listen();

private:
    int _port;

    evutil_socket_t _listenerFd;
    struct event *_listenerEvent;
    ListenerEventArg *_listenerEventArg;
};

struct ListenerEventArg
{
    struct event_base *base;
    Acceptor *acceptor;
};

} /* namespace Nwg */

#endif /* NWG_ACCEPTOR_H_ */
