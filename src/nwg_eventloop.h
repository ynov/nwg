#ifndef NWG_EVENTLOOP_H_
#define NWG_EVENTLOOP_H_

#include <event2/event.h>

namespace Nwg
{

class EventLoop
{
public:
    EventLoop();
    virtual ~EventLoop();

    struct event_base *getBase() const;
    void dispatch();

private:
    struct event_base *_base;
};

} /* namespace Nwg */

#endif /* NWG_EVENTLOOP_H_ */
