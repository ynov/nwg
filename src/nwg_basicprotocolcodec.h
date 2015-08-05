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

    void transformUp(MessageBuffer *in, MessageBuffer **out) const;
    void transformDown(MessageBuffer *in, MessageBuffer *out) const;
};

} /* namespace Nwg */

#endif /* NWG_BASICPROTOCOLCODEC_H_ */
