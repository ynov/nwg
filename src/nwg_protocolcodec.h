#ifndef NWG_PROTOCOLCODEC_H_
#define NWG_PROTOCOLCODEC_H_

#include "nwg_objectcontainer.h"

namespace Nwg
{

class ByteBuffer;

class ProtocolCodec
{
public:
    ProtocolCodec();
    virtual ~ProtocolCodec();

    virtual void transformUp(ByteBuffer *in, ObjectContainer *out) const = 0;
    virtual void transformDown(Object *in, ByteBuffer *out) const = 0;
};

} /* namespace Nwg */

#endif /* NWG_PROTOCOLCODEC_H_ */
