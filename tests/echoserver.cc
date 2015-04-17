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
        int &num_client = ++session.getServer().get<int>("num_client");
        int &client_id = ++session.getServer().get<int>("last_id");

        printf("client #%d connected!\n", client_id);
        printf("num_client: %d\n", num_client);

        session.put<int>("msg_received", std::make_shared<int>(1));
        session.put<int>("client_id", std::make_shared<int>(client_id));

        if (num_client > 2) {
            session.close();
            return;
        }

        Nwg::ByteBuffer &out = *new Nwg::ByteBuffer(BUFFSIZE);
        out.putString("Just type anything. Send key ^] to exit from telnet.\n");
        out.putString("1. << ");
        out.flip();

        session.write(&out);
    }

    void sessionClosed(Nwg::Session &session)
    {
        int &msg_received = session.get<int>("msg_received");
        int &num_client = --session.getServer().get<int>("num_client");
        int &client_id = session.get<int>("client_id");

        printf("client #%d disconnected! # of message received: %d\n", client_id, --msg_received);
        printf("num_client: %d\n", num_client);
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &message)
    {
        Nwg::ByteBuffer &b = dynamic_cast<Nwg::ByteBuffer &>(message);

        int &msg_received = session.get<int>("msg_received");
        int &client_id = session.get<int>("client_id");

        printf("client #%d >> %s\n", client_id, b.getString(b.remaining()).c_str());
        b.flip();

        Nwg::ByteBuffer &out = *new Nwg::ByteBuffer(BUFFSIZE);
        out.putString(std::to_string(msg_received) + ". >> ");
        out.putBytes(b.getBytes(b.remaining()));
        out.putString("\n" + std::to_string(msg_received + 1) + ". << ");
        out.flip();

        session.write(&out);

        msg_received++;
    }

    void messageSent(Nwg::Session &session, Nwg::Object &message)
    {

    }
};


void run()
{
    Nwg::Server server(8845);
    server.setBuffSize(BUFFSIZE);

    server.setProtocolCodec(new Nwg::BasicProtocolCodec());
    server.setHandler(new EchoHandler());

    server.put<int>("last_id", std::make_shared<int>(0));
    server.put<int>("num_client", std::make_shared<int>(0));

    printf("Listening on port %d\n", server.getPort());
    server.run();
}

int main()
{
    run();
    return 0;
}
