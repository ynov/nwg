#ifndef NWG_SERVER_H_
#define NWG_SERVER_H_

#include <map>
#include <memory>

#include "nwg_object.h"
#include "nwg_handler.h"
#include "nwg_protocolcodec.h"

namespace Nwg
{

class Server : public Object
{
public:
    Server();
    virtual ~Server();

    std::map<std::string, std::shared_ptr<Object>> &globals();
    void setProtocolCodec(std::shared_ptr<ProtocolCodec> _protocolCodec);
    void setHandler(std::shared_ptr<Handler> _handler);

    std::shared_ptr<Handler> handler();
    std::shared_ptr<ProtocolCodec> protocolCodec();

private:
    std::map<std::string, std::shared_ptr<Object>> _globals;
    std::shared_ptr<ProtocolCodec> _protocolCodec;
    std::shared_ptr<Handler> _handler;
};

} /* namespace Nwg */

#endif /* NWG_SERVER_H_ */
