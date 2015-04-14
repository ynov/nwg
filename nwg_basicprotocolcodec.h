#ifndef NWG_BASICPROTOCOLCODEC_H_
#define NWG_BASICPROTOCOLCODEC_H_

#include "nwg_protocolcodec.h"

namespace Nwg
{

class BasicProtocolCodec : public ProtocolCodec
{
public:
    BasicProtocolCodec();
    virtual ~BasicProtocolCodec();

    void encode(ByteBuffer &in, ObjectContainer &out) const;
    void decode(Object &in, ByteBuffer &out) const;
};

} /* namespace Nwg */

#endif /* NWG_BASICPROTOCOLCODEC_H_ */
