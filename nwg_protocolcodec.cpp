#include "nwg_protocolcodec.h"

namespace Nwg
{

ProtocolCodec::ProtocolCodec()
{
}

ProtocolCodec::~ProtocolCodec()
{
}

void ProtocolCodec::encode(ByteBuffer &in, ObjectContainer &out)
{
    std::shared_ptr<ByteBuffer> inCopy(new ByteBuffer(in.remaining()));
    inCopy->putBytes(in.getBytes(in.remaining()));
    inCopy->flip();
    out.setObject(inCopy);
}

void ProtocolCodec::decode(Object &in, ByteBuffer &out)
{
    ByteBuffer &b = dynamic_cast<ByteBuffer>(in);
    out.putBytes(b.getBytes(b.remaining()));
    out.flip();
}

} /* namespace Nwg */
