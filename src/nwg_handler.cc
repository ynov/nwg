#include "nwg_handler.h"
#include "nwg_session.h"

namespace Nwg
{

Handler::Handler()
{
}

Handler::~Handler()
{
}

void Handler::sessionOpened(Session &session)
{
}

void Handler::sessionClosed(Session &session)
{
}

void Handler::messageReceived(Session &session, Object &message)
{
}

void Handler::messageSent(Session &session, Object &message)
{
}

void Handler::errorCaught(Session &session, int errorCode, int errNo)
{
}

} /* namespace Nwg */
