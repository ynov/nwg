#include <cstdio>

#include "nwg_server.h"
#include "nwg_basicprotocolcodec.h"

#define BUFFSIZE 32768

template<class T>
T &get(std::shared_ptr<void> p) {
    return *(T *) p.get();
}

class EchoHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        printf("One client connected!\n");

        session.put<int>("i", std::make_shared<int>(1));

#if 1
        Nwg::ByteBuffer *out = new Nwg::ByteBuffer(BUFFSIZE);
        out->putString("Just type anything. Send key ^] to exit from telnet.\n");
        out->flip();

        session.write(out);
#endif
    }

    void sessionClosed(Nwg::Session &session)
    {
        int &i = session.get<int>("i");

        printf("One client disconnected! # of message sent: %d\n", --i);
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &message)
    {
        Nwg::ByteBuffer &b = dynamic_cast<Nwg::ByteBuffer &>(message);

        int &i = session.get<int>("i");

        printf("\n >> %s\n", b.getString(b.remaining()).c_str());
        b.flip();

        Nwg::ByteBuffer *out = new Nwg::ByteBuffer(BUFFSIZE);
        out->putString(std::to_string(i) + " >> ");
        out->putBytes(b.getBytes(b.remaining()));
        out->flip();

        i++;

        session.write(out);
    }

    void messageSent(Nwg::Session &session, Nwg::Object &message)
    {
        // session.close();
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
