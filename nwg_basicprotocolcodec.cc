#include "nwg_basicprotocolcodec.h"
#include "nwg_bytebuffer.h"

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
    out->put(decoded->read(decoded->remaining()));
    out->flip();

    decoded->flip();
}

} /* namespace Nwg */
