#ifndef NWG_HANDLER_H_
#define NWG_HANDLER_H_

namespace Nwg
{

class MessageBuffer;
class Session;

class Handler
{
public:
    Handler();
    virtual ~Handler();

    virtual void sessionOpened(Session &session);
    virtual void sessionClosed(Session &session);
    virtual void messageReceived(Session &session, MessageBuffer &message);
    virtual void messageSent(Session &session, MessageBuffer &message);
    virtual void errorCaught(Session &session, int errorCode, int errNo=-1);
};

} /* namespace Nwg */

#endif /* NWG_HANDLER_H_ */
