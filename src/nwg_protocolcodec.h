#ifndef NWG_PROTOCOLCODEC_H_
#define NWG_PROTOCOLCODEC_H_

namespace Nwg
{

class MessageBuffer;

class ProtocolCodec
{
public:
    ProtocolCodec();
    virtual ~ProtocolCodec();

    virtual void transformUp(MessageBuffer *in, MessageBuffer **out) const = 0;
    virtual void transformDown(MessageBuffer *in, MessageBuffer *out) const = 0;
};

} /* namespace Nwg */

#endif /* NWG_PROTOCOLCODEC_H_ */
