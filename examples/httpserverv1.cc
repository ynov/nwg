#include <cstdio>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

#define BUFFSIZE 32768

#define SILENT 1

#if SILENT
#define _printf(...)
#else
#define _printf(...) fprintf (stdout, __VA_ARGS__)
#endif

class HttpHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        int req_no = ++session.getServer().get<int>("num_req");
        session.put<int>("req_no", std::make_shared<int>(req_no));

        _printf("==== SESSION OPENED #%d ====\n", req_no);
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = dynamic_cast<Nwg::ByteBuffer &>(obj);

        _printf("==== REQUEST RECEIVED ====\n");
        _printf("%s\n", msg.sread(msg.remaining()).c_str());

        Nwg::ByteBuffer &out = *new Nwg::ByteBuffer(BUFFSIZE);
        out.put("HTTP/1.1 200 OK\r\n");
        out.put("Content-Type: text/html\r\n");
        out.put("Connection: close\r\n");
        out.put("\r\n");

        out.put("<!DOCTYPE html>\n");
        out.put("<html>\n");
        out.put("<head>\n");
        out.put("    <meta charset=\"utf-8\"/>\n");
        out.put("    <title>nwg &raquo; examples &raquo; httpserverv1</title>\n");
        out.put("</head>\n");
        out.put("<body>\n");
        out.put("    <h1>The quick brown fox jumps over the lazy dog.</h1>\n");
        out.put("</body>\n");
        out.put("</html>\n");

        out.flip();

        session.write(&out);
    }

    void messageSent(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = dynamic_cast<Nwg::ByteBuffer &>(obj);

        _printf("==== RESPONSE SENT ====\n");
        _printf("%s\n", msg.sread(msg.remaining()).c_str());

        session.close();
    }

    void sessionClosed(Nwg::Session &session)
    {
        int req_no = session.get<int>("req_no");
        _printf("==== SESSION CLOSED #%d ====\n\n", req_no);
    }
};

void run()
{
    Nwg::Server server(8840);
    server.setBuffSize(BUFFSIZE);

    server.setProtocolCodec(new Nwg::BasicProtocolCodec());
    server.setHandler(new HttpHandler());

    server.put<int>("num_req", std::make_shared<int>(0));

    printf("Listening on port %d\n", server.getPort());
    printf("Open http://127.0.0.1:%d/\n", server.getPort());
    server.run();
}

int main()
{
    run();
    return 0;
}
