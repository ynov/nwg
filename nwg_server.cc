#include "nwg_server.h"

namespace Nwg
{

Server::Server()
    : _protocolCodec(nullptr), _handler(nullptr)
{
}

Server::~Server()
{
}

std::map<std::string, std::shared_ptr<Object>> &Server::globals()
{
    return _globals;
}

void Server::setProtocolCodec(std::shared_ptr<ProtocolCodec> protocolCodec)
{
    _protocolCodec = protocolCodec;
}

void Server::setHandler(std::shared_ptr<Handler> handler)
{
    _handler = handler;
}

std::shared_ptr<ProtocolCodec> Server::protocolCodec()
{
    return _protocolCodec;
}


std::shared_ptr<Handler> Server::handler()
{
    return _handler;
}

} /* namespace Nwg */
