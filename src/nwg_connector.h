#ifndef NWG_CONNECTOR_H_
#define NWG_CONNECTOR_H_

#include "nwg_service.h"

namespace Nwg
{

struct ConnectorEventArg;

class Connector : public Service
{
public:
    Connector(EventLoop *eventLoop);
    virtual ~Connector();

    struct event *getConnectorEvent();
    bool connect(const std::string &hostip, int port, int timeoutSecond = 5);

private:
    evutil_socket_t _serverFd;
    struct event *_connectorEvent;
    ConnectorEventArg *_connectorEventArg;
    struct timeval *_timeout;
};

struct ConnectorEventArg
{
    struct event_base *base;
    Connector *connector;
};

} /* namespace Nwg */

#endif /* NWG_CONNECTOR_H_ */
