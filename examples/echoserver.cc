#include <cstdio>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

#define BUFFSIZE SZ_32KB
#define MAX_CLIENT 5

static int numClient = 0;
static int lastId = 0;

class EchoHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        std::shared_ptr<Nwg::MessageBuffer> out(new Nwg::MessageBuffer(BUFFSIZE));

        numClient++;
        lastId++;

        std::shared_ptr<int> clientId(new int(lastId));

        printf("client #%d connected!\n", *clientId);
        printf("num_client: %d\n", numClient);

        session.put<int>("msg_received", std::make_shared<int>(1));
        session.put<int>("client_id", clientId);

        if (numClient > MAX_CLIENT) {
            out->put("Client limit exceeded!\n");
            out->flip();

            session.write(out);
            return;
        } else {
            out->put("Just type anything. Send key ^] to exit from telnet.\n");
            out->put("1. << ");
            out->flip();

            session.write(out);
            return;
        }
    }

    void sessionClosed(Nwg::Session &session)
    {
        --numClient;

        int clientId = session.get<int>("client_id");
        int msgReceived = --session.get<int>("msg_received");

        printf("client #%d disconnected! # of message received: %d\n", clientId, msgReceived);
        printf("num_client: %d\n", numClient);
    }

    void messageReceived(Nwg::Session &session, Nwg::MessageBuffer &obj)
    {
        Nwg::MessageBuffer &msg = dynamic_cast<Nwg::MessageBuffer &>(obj);
        std::shared_ptr<Nwg::MessageBuffer> out(new Nwg::MessageBuffer(BUFFSIZE));

        int clientId = session.get<int>("client_id");
        int &msgReceived = session.get<int>("msg_received");

        printf("client #%d >> %s\n", clientId, msg.sreadUntil('\n').c_str());
        msg.jump(0);

        out->put(std::to_string(msgReceived) + ". >> ");
        out->put(msg.read(msg.remaining()));
        out->put("\n" + std::to_string(msgReceived + 1) + ". << ");
        out->flip();

        session.write(out);

        msgReceived++;
    }

    void messageSent(Nwg::Session &session, Nwg::MessageBuffer &obj)
    {
        if (numClient > MAX_CLIENT) {
            session.close();
        }
    }
};

void run(int port)
{
    Nwg::EventLoop eventLoop;
    Nwg::Acceptor acceptor(&eventLoop, port);

    acceptor.setBuffSize(BUFFSIZE);
    acceptor.setProtocolCodec(std::make_shared<Nwg::BasicProtocolCodec>());
    acceptor.setHandler(std::make_shared<EchoHandler>());

    printf("Listening on port %d\n", acceptor.getPort());
    acceptor.listen();

    eventLoop.dispatch();
}

int main(int argc, char **argv)
{
    run([&]() -> int {
        if (argc > 1) {
            return std::stoi(argv[1]);
        } else {
            return 8840;
        }
    }());
    return 0;
}
