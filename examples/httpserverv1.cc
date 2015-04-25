#include <cstdio>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

#define BUFFSIZE SZ_32KB

#ifndef SILENT
#define SILENT 1
#endif

#if SILENT
#define _printf(...)
#else
#define _printf(...) fprintf (stdout, __VA_ARGS__)
#endif

#define GETMSG(obj) dynamic_cast<Nwg::ByteBuffer &>(obj)

static int numReq = 0;

class HttpHandler : public Nwg::Handler
{
    void sessionOpened(Nwg::Session &session)
    {
        int reqNo = ++numReq;
        session.put<int>("req_no", std::make_shared<int>(reqNo));

        _printf("==== SESSION OPENED #%d ====\n", reqNo);
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = GETMSG(obj);
        std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer(BUFFSIZE));

        _printf("==== REQUEST RECEIVED ====\n");
        _printf("%s\n", msg.sread(msg.remaining()).c_str());

        out->put("HTTP/1.1 200 OK\r\n");
        out->put("Content-Type: text/html\r\n");
        out->put("Connection: close\r\n");
        out->put("\r\n");

        out->put("<!DOCTYPE html>\n");
        out->put("<html>\n");
        out->put("<head>\n");
        out->put("    <meta charset=\"utf-8\"/>\n");
        out->put("    <title>nwg &raquo; examples &raquo; httpserverv1</title>\n");
        out->put("</head>\n");
        out->put("<body>\n");
        out->put("    <h1>The quick brown fox jumps over the lazy dog.</h1>\n");
        out->put("    <pre>request #" + std::to_string(session.get<int>("req_no")) + "</pre>\n");
        out->put("</body>\n");
        out->put("</html>\n");

        out->flip();

        session.write(out);
    }

    void messageSent(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = GETMSG(obj);

        _printf("==== RESPONSE SENT ====\n");
        _printf("%s\n", msg.sread(msg.remaining()).c_str());

        session.close();
    }

    void sessionClosed(Nwg::Session &session)
    {
        _printf("==== SESSION CLOSED #%d ====\n\n", session.get<int>("req_no"));
    }
};

void run()
{
    Nwg::Acceptor acceptor(8840);
    acceptor.setBuffSize(BUFFSIZE);

    acceptor.setProtocolCodec(std::make_shared<Nwg::BasicProtocolCodec>());
    acceptor.setHandler(std::make_shared<HttpHandler>());

    printf("Listening on port %d\n", acceptor.getPort());
    printf("Open http://127.0.0.1:%d/\n", acceptor.getPort());

    acceptor.listen(true);
}

int main()
{
    run();
    return 0;
}
