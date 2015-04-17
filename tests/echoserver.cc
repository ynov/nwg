#include <cstdio>

#include "nwg_server.h"
#include "nwg_basicprotocolcodec.h"

#define BUFFSIZE 32768
#define MAX_CLIENT 5

class EchoHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        const int num_client = ++session.getServer().get<int>("num_client");
        const int last_id = ++session.getServer().get<int>("last_id");
        int &client_id = *new int(last_id);

        printf("client #%d connected!\n", client_id);
        printf("num_client: %d\n", num_client);

        session.put<int>("msg_received", std::make_shared<int>(1));
        session.put<int>("client_id", &client_id);

        Nwg::ByteBuffer &out = *new Nwg::ByteBuffer(BUFFSIZE);

        if (num_client > MAX_CLIENT) {
            out.putString("Client limit exceeded!\n");
            out.flip();

            session.write(&out);
            return;
        }

        out.putString("Just type anything. Send key ^] to exit from telnet.\n");
        out.putString("1. << ");
        out.flip();

        session.write(&out);
    }

    void sessionClosed(Nwg::Session &session)
    {
        const int num_client = --session.getServer().get<int>("num_client");
        const int client_id = session.get<int>("client_id");
        const int msg_received = --session.get<int>("msg_received");

        printf("client #%d disconnected! # of message received: %d\n", client_id, msg_received);
        printf("num_client: %d\n", num_client);
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = dynamic_cast<Nwg::ByteBuffer &>(obj);

        const int client_id = session.get<int>("client_id");
        int &msg_received = session.get<int>("msg_received");

        printf("client #%d >> %s\n", client_id, msg.sreadUntil('\n').c_str());
        msg.jump(0);

        Nwg::ByteBuffer &out = *new Nwg::ByteBuffer(BUFFSIZE);
        out.putString(std::to_string(msg_received) + ". >> ");
        out.putBytes(msg.read(msg.remaining()));
        out.putString("\n" + std::to_string(msg_received + 1) + ". << ");
        out.flip();

        session.write(&out);

        msg_received++;
    }

    void messageSent(Nwg::Session &session, Nwg::Object &obj)
    {
        const int num_client = session.getServer().get<int>("num_client");

        if (num_client > MAX_CLIENT) {
            session.close();
        }
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
