#include <cstdio>

#include "nwg_server.h"
#include "nwg_basicprotocolcodec.h"

class EchoHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        printf("One client connected!\n");
    }

    void sessionClosed(Nwg::Session &session)
    {
        printf("One client disconnected!\n");
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &message)
    {
        Nwg::ByteBuffer &b = dynamic_cast<Nwg::ByteBuffer &>(message);

        Nwg::ByteBuffer bCopy = b;
        printf("\n >> %s\n", bCopy.getString(bCopy.remaining()).c_str());

        Nwg::ByteBuffer *out = new Nwg::ByteBuffer(session.bufferAllocationSize());
        out->putBytes(b.getBytes(b.remaining()));
        out->flip();

        session.write(out);
    }

    void messageSent(Nwg::Session &session, Nwg::Object &message)
    {
        Nwg::ByteBuffer &b = dynamic_cast<Nwg::ByteBuffer &>(message);
        session.close();
    }
};


void run()
{
    Nwg::Server server(8845);

    std::shared_ptr<Nwg::ProtocolCodec> protocolCodec(new Nwg::BasicProtocolCodec());
    std::shared_ptr<Nwg::Handler> handler(new EchoHandler());

    server.setProtocolCodec(protocolCodec);
    server.setHandler(handler);

    printf("Listening on port %d\n", server.port());
    server.run();
}

int main()
{
    run();
    return 0;
}
