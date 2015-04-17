#include "nwg_basicprotocolcodec.h"

namespace Nwg
{

BasicProtocolCodec::BasicProtocolCodec()
{
}

BasicProtocolCodec::~BasicProtocolCodec()
{
}

void BasicProtocolCodec::encode(ByteBuffer *in, ObjectContainer *out) const
{
    ByteBuffer *encoded = new ByteBuffer(in->ssize());
    *encoded = *in;
    out->setObject(encoded);
}

void BasicProtocolCodec::decode(Object *in, ByteBuffer *out) const
{
    if (in == nullptr) {
        return;
    }

    ByteBuffer *decoded = dynamic_cast<ByteBuffer *>(in);

    out->reset();
    out->putBytes(decoded->getBytes(decoded->remaining()));
    out->flip();
}

} /* namespace Nwg */
