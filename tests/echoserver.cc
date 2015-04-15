#include <cstdio>

#include "nwg_server.h"
#include "nwg_basicprotocolcodec.h"

#define BUFFSIZE 32768

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

        printf("\n >> %s\n", b.getString(b.remaining()).c_str());
        b.flip();

        Nwg::ByteBuffer *out = new Nwg::ByteBuffer(session.getBufferAllocationSize());
        out->putBytes(b.getBytes(b.remaining()));
        out->flip();

        session.write(out);
    }

    void messageSent(Nwg::Session &session, Nwg::Object &message)
    {
        session.close();
    }
};


void run()
{
    Nwg::Server server(8845);
    server.setBuffSize(BUFFSIZE);

    server.setProtocolCodec(new Nwg::BasicProtocolCodec());
    server.setHandler(new EchoHandler());

    printf("Listening on port %d\n", server.getPort());
    server.run();
}

int main()
{
    run();
    return 0;
}
