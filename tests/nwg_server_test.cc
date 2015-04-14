#include <cstdio>

#include "nwg_server.h"
#include "nwg_basicprotocolcodec.h"

class DummyHandler : public Nwg::Handler
{
    void messageReceived(Nwg::Session &session, Nwg::Object &message)
    {
        printf("On messageReceived()!\n");

        Nwg::ByteBuffer &b = dynamic_cast<Nwg::ByteBuffer &>(message);
        printf("In Message: %s\n", b.getString(b.remaining()).c_str());
    }

    void messageSent(Nwg::Session &session, Nwg::Object &message)
    {
        printf("On messageSent()!\n");

        Nwg::ByteBuffer &b = dynamic_cast<Nwg::ByteBuffer &>(message);
        printf("Out Message: %s\n", b.getString(b.remaining()).c_str());
    }
};

class BasicServer : public Nwg::Server
{
public:
    void wtf()
    {
        Nwg::Session session(4096);

        Nwg::ByteBuffer &in = session.getReadBuffer();
        Nwg::ByteBuffer &out = session.getWriteBuffer();

        in.putString("The quick brown fox jumps over the lazy dog.");
        in.flip();

        Nwg::ObjectContainer oc;

        protocolCodec()->encode(in, oc);
        handler()->messageReceived(session, *oc.object());

        std::shared_ptr<Nwg::ByteBuffer> writeObject(new Nwg::ByteBuffer(4096));
        writeObject->putString("Grumpy wizards make toxic brew for the evil queen and jack.");
        writeObject->flip();

        session.write(writeObject);

        protocolCodec()->decode(session.getWriteObject(), out);
        Nwg::ByteBuffer outCopy = out;
        printf(" >> %s\n", out.getString(out.remaining()).c_str());
        handler()->messageSent(session, outCopy);
    }
};

void test_dummy()
{
    printf("-- BEGIN test_dummy() --\n\n");
    BasicServer server;

    std::shared_ptr<Nwg::ProtocolCodec> protocolCodec(new Nwg::BasicProtocolCodec());
    std::shared_ptr<Nwg::Handler> handler(new DummyHandler());

    server.setProtocolCodec(protocolCodec);
    server.setHandler(handler);

    server.wtf();

    printf("\n-- END test_dummy() --\n");
}

int main()
{
    test_dummy();

    return 0;
}
