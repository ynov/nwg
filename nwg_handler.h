#ifndef NWG_HANDLER_H_
#define NWG_HANDLER_H_

#include "nwg_object.h"

namespace Nwg
{

class Session;

class Handler: public Object
{
public:
    Handler();
    virtual ~Handler();

    virtual void sessionOpened(Session &session);
    virtual void sessionClosed(Session &session);
    virtual void messageReceived(Session &session, Object &message);
    virtual void messageSent(Session &session, Object &message);
};

} /* namespace Nwg */

#endif /* NWG_HANDLER_H_ */
