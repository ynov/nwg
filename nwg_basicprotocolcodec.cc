#include "nwg_basicprotocolcodec.h"

namespace Nwg
{

BasicProtocolCodec::BasicProtocolCodec()
{
}

BasicProtocolCodec::~BasicProtocolCodec()
{
}

void BasicProtocolCodec::encode(ByteBuffer &in, ObjectContainer &out) const
{
    ByteBuffer *inCopy = new ByteBuffer(in.remaining());
    inCopy->putBytes(in.getBytes(in.remaining()));
    inCopy->flip();
    out.setObject(inCopy);
}

void BasicProtocolCodec::decode(Object &in, ByteBuffer &out) const
{
    if (&in == nullptr) {
        return;
    }

    ByteBuffer &b = dynamic_cast<ByteBuffer &>(in);
    out.putBytes(b.getBytes(b.remaining()));
    out.flip();
}

} /* namespace Nwg */
