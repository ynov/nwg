#include <cstdio>

#include "nwg_server.h"
#include "nwg_basicprotocolcodec.h"

#define BUFFSIZE 4096

class DummyHandler : public Nwg::Handler
{
    void messageReceived(Nwg::Session &session, Nwg::Object &message)
    {
        printf("On messageReceived()!\n");

        Nwg::ByteBuffer &b = dynamic_cast<Nwg::ByteBuffer &>(message);
        printf("In Message: %s\n", b.sread(b.remaining()).c_str());

        Nwg::ByteBuffer *out = new Nwg::ByteBuffer(session.getBufferAllocationSize());
        out->put("Grumpy wizards make toxic brew for the evil queen and jack.");
        out->flip();

        session.write(out);
    }

    void messageSent(Nwg::Session &session, Nwg::Object &message)
    {
        printf("On messageSent()!\n");

        Nwg::ByteBuffer &b = dynamic_cast<Nwg::ByteBuffer &>(message);
        printf("Out Message: %s\n", b.sread(b.remaining()).c_str());
    }
};

class DummyServer : public Nwg::Server
{
public:
    DummyServer() : Nwg::Server(8845) {}

    void wtf()
    {
        Nwg::Session session(BUFFSIZE, nullptr, 0, nullptr);

        Nwg::ByteBuffer &in = session.getReadBuffer();
        Nwg::ByteBuffer &out = session.getWriteBuffer();

        in.put("The quick brown fox jumps over the lazy dog.");
        in.flip();

        Nwg::ObjectContainer oc;

        getProtocolCodec().encode(&in, &oc);
        getHandler().messageReceived(session, oc.getObject());

        getProtocolCodec().decode(&session.getWriteObject(), &out);
        Nwg::ByteBuffer outCopy = out;
        printf(" >> %s\n", out.sread(out.remaining()).c_str());
        getHandler().messageSent(session, outCopy);
    }
};

void test_dummy()
{
    printf("-- BEGIN test_dummy() --\n\n");

    DummyServer server;
    server.setBuffSize(BUFFSIZE);

    server.setProtocolCodec(new Nwg::BasicProtocolCodec());
    server.setHandler(new DummyHandler());

    server.wtf();

    printf("\n-- END test_dummy() --\n");
}

int main()
{
    test_dummy();

    return 0;
}
