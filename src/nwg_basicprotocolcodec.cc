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

void BasicProtocolCodec::transformUp(ByteBuffer *in, ObjectContainer *out) const
{
    std::unique_ptr<ByteBuffer> encoded(new ByteBuffer(in->ssize()));
    *encoded = *in;
    out->setObject(std::move(encoded));
}

void BasicProtocolCodec::transformDown(Object *in, ByteBuffer *out) const
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
