#include "nwg_server.h"

namespace Nwg
{

Server::Server()
{
}

Server::~Server()
{
}

std::map<std::string, std::shared_ptr<Object>> &Server::globals()
{
    return _globals;
}

} /* namespace Nwg */
