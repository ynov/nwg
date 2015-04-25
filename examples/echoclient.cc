#include <cstdio>

#include <iostream>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

#define BUFFSIZE SZ_32KB

static int msgNo = 0;

class EchoClientHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        printf("Session opened!\n");
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = dynamic_cast<Nwg::ByteBuffer &>(obj);

        printf("Message: %s\n", msg.sreadUntil('\n').c_str());

        std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer(BUFFSIZE));

        if (msgNo == 0) {
            msgNo++;

            out->put("The quick brown fox jumps over the lazy dog.\n");
            out->flip();

            session.write(out);
        } else if (msgNo == 1) {
            msgNo++;

            out->put("Grumpy wizards make toxic brew for the evil queen and jack.\n");
            out->flip();

            session.write(out);
        } else if (msgNo == 2) {
            msgNo++;

            std::string input;
            printf("Enter message: ");

            std::getline(std::cin, input);

            out->put(input.c_str());
            out->flip();

            session.write(out);
        } else {
            session.close();
        }

    }

    void messageSent(Nwg::Session &session, Nwg::Object &obj)
    {

    }

    void sessionClosed(Nwg::Session &session)
    {
        printf("Session closed!\n");
    }

    void errorCaught(Nwg::Session &session, int errorCode, int errNo)
    {
        switch (errorCode)
        {
        case NWG_ECONNREFUSED:
            printf("Connection refused! errNo = %d\n", errNo);
            break;
        default:
            printf("ERROR caught! errorCode = %d; errNo = %d\n", errorCode, errNo);
            break;
        }
    }
};

void run(const std::string &hostname, int port)
{
    Nwg::EventLoop eventLoop;
    Nwg::Connector connector(&eventLoop);

    connector.setBuffSize(BUFFSIZE);
    connector.setProtocolCodec(std::make_shared<Nwg::BasicProtocolCodec>());
    connector.setHandler(std::make_shared<EchoClientHandler>());

    printf("Connecting to %s:%d...\n", hostname.c_str(), port);
    if (!connector.connect(hostname, port, 5)) {
        printf("Unable to connect.\n");
    }

    eventLoop.dispatch();
}

int main(int argc, char **argv)
{
    std::string hostname = [&]() -> const char * {
       if (argc > 1) {
           return argv[1];
       } else {
           return "127.0.0.1";
       }
    }();

    int port = [&]() {
        if (argc > 2) {
            return std::stoi(argv[2]);
        } else {
            return 8845;
        }
    }();

    run(hostname, port);
    return 0;
}
