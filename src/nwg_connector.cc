#include "nwg_common_socket_include.h"

#include <cstring>

#include "nwg_connector.h"
#include "nwg_evcb.h"

namespace Nwg
{

Connector::Connector(const Service *service)
    : Service(service),
      _serverFd(-1),
      _connectorEvent(nullptr),
      _connectorEventArg(nullptr)
{
}

Connector::~Connector()
{
}

struct event *Connector::getConnectorEvent()
{
    return _connectorEvent;
}

bool Connector::connect(const std::string &hostname, int port, bool dispatch)
{
    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(port);

    if (::inet_pton(AF_INET, hostname.c_str(), &sin.sin_addr) <= 0) {
        perror("inet_pton()");
        return false;
    }

    _serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
    evutil_make_socket_nonblocking(_serverFd);

    if (::connect(_serverFd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        bool err_einprogress = false;

#ifdef __unix__
        err_einprogress = errno == EINPROGRESS;
#endif /* __unix__ */

#ifdef _WIN32
        errno = WSAGetLastError();
        err_einprogress = errno == WSAEWOULDBLOCK;
#endif /* _WIN32 */

        if (!err_einprogress) {
            perror("connect()");
            printf("errno = %d\n", errno);
            return false;
        }
    }

    _connectorEventArg = new ConnectorEventArg();
    _connectorEventArg->base      = _base;
    _connectorEventArg->connector = this;

    _connectorEvent = event_new(_base, _serverFd,
            EV_READ | EV_WRITE,
            EVCB::doConnect,
            (void *) _connectorEventArg);

    event_add(_connectorEvent, NULL);

    if (dispatch) {
        event_base_dispatch(_base);
    }

    return true;
}

} /* namespace Nwg */
