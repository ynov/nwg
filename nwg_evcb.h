#ifndef NWG_EVCB_H_
#define NWG_EVCB_H_

#include <event2/event.h>

#define SMALL_BUFFSIZE 1024

namespace Nwg
{

struct EVCB
{
    static void doAccept(evutil_socket_t listener, short event, void *arg);
    static void doConnect(evutil_socket_t fd, short event, void *arg);
    static void doRead(evutil_socket_t fd, short events, void *arg);
    static void doWrite(evutil_socket_t fd, short events, void *arg);
};

} /* namespace Nwg */

#endif /* NWG_EVCB_H_ */
