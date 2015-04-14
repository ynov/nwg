#ifndef NWG_PROTOCOLCODEC_H_
#define NWG_PROTOCOLCODEC_H_

#include "nwg_object.h"

namespace Nwg
{

class ProtocolCodec: public Object
{
public:
    ProtocolCodec();
    virtual ~ProtocolCodec();

    void encode(ByteBuffer &in, ObjectContainer &out);
    void decode(Object &in, ByteBuffer &out);
};

} /* namespace Nwg */

#endif /* NWG_PROTOCOLCODEC_H_ */
