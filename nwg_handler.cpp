#include "nwg_handler.h"

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

} /* namespace Nwg */
