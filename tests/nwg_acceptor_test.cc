#include <cstdio>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

#define BUFFSIZE 4096

class DummyHandler : public Nwg::Handler
{
    void messageReceived(Nwg::Session &session, Nwg::MessageBuffer &message)
    {
        printf("On messageReceived()!\n");

        Nwg::MessageBuffer &b = dynamic_cast<Nwg::MessageBuffer &>(message);
        printf("In Message: %s\n", b.sread(b.remaining()).c_str());

        std::shared_ptr<Nwg::MessageBuffer> out(new Nwg::MessageBuffer(session.getBufferAllocationSize()));
        out->put("Grumpy wizards make toxic brew for the evil queen and jack.");
        out->flip();

        session.write(out);
    }

    void messageSent(Nwg::Session &session, Nwg::MessageBuffer &message)
    {
        printf("On messageSent()!\n");

        Nwg::MessageBuffer &b = dynamic_cast<Nwg::MessageBuffer &>(message);
        printf("Out Message: %s\n", b.sread(b.remaining()).c_str());
    }
};

class DummyAcceptor : public Nwg::Acceptor
{
public:
    DummyAcceptor() : Nwg::Acceptor(nullptr) {}

    void wtf()
    {
        Nwg::Session session(BUFFSIZE, nullptr, 0, nullptr);

        Nwg::MessageBuffer &in = session.getReadBuffer();
        Nwg::MessageBuffer &out = session.getWriteBuffer();

        in.put("The quick brown fox jumps over the lazy dog.");
        in.flip();

        Nwg::MessageBuffer *outBuff;

        getProtocolCodec().transformUp(&in, &outBuff);
        getHandler().messageReceived(session, *outBuff);

        getProtocolCodec().transformDown(&session.getWriteObject(), &out);
        Nwg::MessageBuffer outCopy = out;
        printf(" >> %s\n", out.sread(out.remaining()).c_str());
        getHandler().messageSent(session, outCopy);
    }
};

void test_dummy()
{
    printf("-- BEGIN test_dummy() --\n\n");

    DummyAcceptor acceptor;
    acceptor.setBuffSize(BUFFSIZE);

    acceptor.setProtocolCodec(std::make_shared<Nwg::BasicProtocolCodec>());
    acceptor.setHandler(std::make_shared<DummyHandler>());

    acceptor.wtf();

    printf("\n-- END test_dummy() --\n");
}

int main()
{
    test_dummy();

    return 0;
}
