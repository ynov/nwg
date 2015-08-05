#include "nwg_basicprotocolcodec.h"

#include <memory>
#include "nwg_messagebuffer.h"

namespace Nwg
{

BasicProtocolCodec::BasicProtocolCodec()
{
}

BasicProtocolCodec::~BasicProtocolCodec()
{
}

void BasicProtocolCodec::transformUp(MessageBuffer *in, MessageBuffer **out) const
{
    // TODO: Prevent leak!
    MessageBuffer *encoded = new MessageBuffer(in->ssize());
    *encoded = *in;

    *out = encoded;
}

void BasicProtocolCodec::transformDown(MessageBuffer *in, MessageBuffer *out) const
{
    if (in == nullptr) {
        return;
    }

    MessageBuffer *decoded = dynamic_cast<MessageBuffer *>(in);

    out->reset();
    out->put(decoded->read(decoded->remaining()));
    out->flip();

    decoded->flip();
}

} /* namespace Nwg */
