#ifndef NWG_ACCEPTOR_H_
#define NWG_ACCEPTOR_H_

#include "nwg_service.h"

namespace Nwg
{

struct ListenerEventArg;

class Acceptor : public Service
{
public:
    Acceptor(int port, const Service *service = nullptr);
    virtual ~Acceptor();

    void setPort(int port);
    int getPort();

    void listen(bool dispatch = false);

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
