#ifndef NWG_SERVER_H_
#define NWG_SERVER_H_

#include <map>
#include <memory>

#include "nwg_object.h"

namespace Nwg
{

class Server: public Object
{
public:
    Server();
    virtual ~Server();

    std::map<std::string, std::shared_ptr<Object>> &globals();

private:
    std::map<std::string, std::shared_ptr<Object>> _globals;
};

} /* namespace Nwg */

#endif /* NWG_SERVER_H_ */
