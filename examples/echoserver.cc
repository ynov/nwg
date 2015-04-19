#include <cstdio>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

#define BUFFSIZE 32768
#define MAX_CLIENT 5

static int numClient = 0;
static int lastId = 0;

class EchoHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        numClient++;
        lastId++;

        int &clientId = *new int(lastId);

        printf("client #%d connected!\n", clientId);
        printf("num_client: %d\n", numClient);

        session.put<int>("msg_received", std::make_shared<int>(1));
        session.put<int>("client_id", &clientId);

        Nwg::ByteBuffer &out = *new Nwg::ByteBuffer(BUFFSIZE);

        if (numClient > MAX_CLIENT) {
            out.put("Client limit exceeded!\n");
            out.flip();

            session.write(&out);
            return;
        }

        out.put("Just type anything. Send key ^] to exit from telnet.\n");
        out.put("1. << ");
        out.flip();

        session.write(&out);
    }

    void sessionClosed(Nwg::Session &session)
    {
        --numClient;

        int clientId = session.get<int>("client_id");
        int msgReceived = --session.get<int>("msg_received");

        printf("client #%d disconnected! # of message received: %d\n", clientId, msgReceived);
        printf("num_client: %d\n", numClient);
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = dynamic_cast<Nwg::ByteBuffer &>(obj);

        int clientId = session.get<int>("client_id");
        int &msgReceived = session.get<int>("msg_received");

        printf("client #%d >> %s\n", clientId, msg.sreadUntil('\n').c_str());
        msg.jump(0);

        Nwg::ByteBuffer &out = *new Nwg::ByteBuffer(BUFFSIZE);
        out.put(std::to_string(msgReceived) + ". >> ");
        out.put(msg.read(msg.remaining()));
        out.put("\n" + std::to_string(msgReceived + 1) + ". << ");
        out.flip();

        session.write(&out);

        msgReceived++;
    }

    void messageSent(Nwg::Session &session, Nwg::Object &obj)
    {
        if (numClient > MAX_CLIENT) {
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

    printf("Listening on port %d\n", server.getPort());
    server.run();
}

int main()
{
    run();
    return 0;
}
