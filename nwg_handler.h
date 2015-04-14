#ifndef NWG_HANDLER_H_
#define NWG_HANDLER_H_

#include "nwg_object.h"

namespace Nwg
{

class Handler: public Object
{
public:
    Handler();
    virtual ~Handler();

    void sessionOpened(Session &session);
    void sessionClosed(Session &session);
    void messageReceived(Session &session, Object &message);
    void messageSent(Session &session, Object &message);
};

} /* namespace Nwg */

#endif /* NWG_HANDLER_H_ */
