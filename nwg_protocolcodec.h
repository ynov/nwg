#ifndef NWG_PROTOCOLCODEC_H_
#define NWG_PROTOCOLCODEC_H_

#include "nwg_object.h"
#include "nwg_objectcontainer.h"
#include "nwg_bytebuffer.h"

namespace Nwg
{

class ProtocolCodec: public Object
{
public:
    ProtocolCodec();
    virtual ~ProtocolCodec();

    virtual void encode(ByteBuffer &in, ObjectContainer &out) const = 0;
    virtual void decode(Object &in, ByteBuffer &out) const = 0;
};

} /* namespace Nwg */

#endif /* NWG_PROTOCOLCODEC_H_ */
