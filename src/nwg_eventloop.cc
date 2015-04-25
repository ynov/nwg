#include "nwg_common_socket_include.h"

#include "nwg_eventloop.h"

namespace Nwg
{

EventLoop::EventLoop()
    : _base(nullptr)
{
#ifdef _WIN32
    do {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD(2, 2);

        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0) {
            printf("WSAStartup failed with error: %d\n", err);
            return;
        }
    } while(0);
#endif /* _WIN32 */

    _base = event_base_new();
    if (!_base) {
        perror("event_base_new()");
        return;
    }
}

struct event_base *EventLoop::getBase() const
{
    return _base;
}

void EventLoop::dispatch()
{
    event_base_dispatch(_base);
}

EventLoop::~EventLoop()
{
}

} /* namespace Nwg */
