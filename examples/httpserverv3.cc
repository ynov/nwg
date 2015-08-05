#include <cstdio>
#include <iostream>
#include <fstream>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

#define BUFFSIZE     SZ_64KB
#define READBUFFSIZE SZ_4MB

#ifndef SILENT
#define SILENT 0
#endif

#if SILENT
#define _printf(...)
#else
#define _printf(...) fprintf (stdout, __VA_ARGS__)
#endif

using boost::regex;
using boost::regex_match;
using boost::regex_replace;
using boost::smatch;

static int numReq = 0;

#define PATTERN "([a-zA-Z]+) (/[0-9a-zA-Z\\-_,.;:'\"\\[\\]\\(\\)+=!@#$%^&*<>/?~`{}|]*) (HTTP/\\d\\.\\d)(\r|)$"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Protocol Codec
////////////////////////////////////////////////////////////////////////////////////////////////////
class HttpRequest : public Nwg::MessageBuffer
{
public:
    void setMethod(const std::string &method) { _method = method; }
    void setUrl(const std::string &url) { _url = url; }
    void setHttpVersion(const std::string &httpVersion) { _httpVersion = httpVersion; }
    void requestOk() { _requestOk = true; }
    std::string &getMethod() { return _method; }
    std::string &getUrl() { return _url; }
    std::string &getHttpVersion() { return _httpVersion; }
    bool isRequestOk() { return _requestOk; }
private:
    std::string _method;
    std::string _url;
    std::string _httpVersion;
    bool _requestOk = false;
};

class HttpResponse : public Nwg::MessageBuffer
{
public:
    void setStatusCode(const std::string &statusCode) { _statusCode = statusCode; }
    void setContentType(const std::string &contentType) { _contentType = contentType; }
    void setConnection(const std::string &connection) { _connection = connection; }
    void setBody(const std::string &body) { _body = body; }
    std::string &getStatusCode() { return _statusCode; }
    std::string &getContentType() { return _contentType; }
    std::string &getConnection() { return _connection; }
    std::string &getBody() { return _body; }
private:
    std::string _statusCode = "200 OK";
    std::string _contentType = "text/html";
    std::string _connection = "close";
    std::string _body = "";
};

class HttpProtocolCodec : public Nwg::ProtocolCodec
{
public:
    void transformUp(Nwg::MessageBuffer *in, Nwg::MessageBuffer **out) const
    {
        HttpRequest *req = new HttpRequest();

        // HTTP Header's First Line
        std::string h1 = in->sreadUntil('\n');

        smatch what;
        if (regex_match(h1, what, regex(PATTERN))) {
            req->setMethod(what[1].str());
            req->setUrl(what[2].str());
            req->setHttpVersion(what[3].str());
            req->requestOk();
        }

        *out = req;
    }

    void transformDown(Nwg::MessageBuffer *in, Nwg::MessageBuffer *out) const
    {
        HttpResponse *res = dynamic_cast<HttpResponse *>(in);

        out->reset();
        out->put("HTTP/1.1 " + res->getStatusCode() + "\r\n");
        out->put("Content-Type: " + res->getContentType() + "\r\n");

        if (res->getConnection() != "") {
            out->put("Connection: " + res->getConnection() + "\r\n");
        }

        out->put("\r\n");
        out->put(res->getBody());
        out->flip();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Http Handler
////////////////////////////////////////////////////////////////////////////////////////////////////
class HttpHandler : public Nwg::Handler
{
private:
    static boost::regex pattern;

    std::string decodeUrl(const std::string &url)
    {
        std::string nurl = url;

        static regex reSpace("%20");
        nurl = regex_replace(nurl, reSpace, " ");

        return nurl;
    }

public:
    void sessionOpened(Nwg::Session &session)
    {
        int reqNo = ++numReq;
        session.put<int>("req_no", std::make_shared<int>(reqNo));

        _printf("|||| SESSION OPENED #%d\n", reqNo);
    }

    void messageReceived(Nwg::Session &session, Nwg::MessageBuffer &msg)
    {
        // Got request
        HttpRequest &req = dynamic_cast<HttpRequest &>(msg);

        _printf("|||| REQUEST RECEIVED #%d\n", session.get<int>("req_no"));
        if (req.isRequestOk()) {
            _printf("| Method: %s\n", req.getMethod().c_str());
            _printf("| URL: %s\n", req.getUrl().c_str());
            _printf("| HTTP Version: %s\n", req.getHttpVersion().c_str());
        } else {
            _printf("| MALFORMED REQUEST.\n");
        }

        // Build response
        std::shared_ptr<HttpResponse> res(new HttpResponse());

        if (req.isRequestOk()) {
            std::stringstream body;

            body << "<h1>Hi.</h1>\n";
            body << "<ul>\n";
            body << "<li>Method: " << req.getMethod() << "</li>\n";
            body << "<li>URL: " << req.getUrl() << "</li>\n";
            body << "<li>HTTP Version: " << req.getHttpVersion() << "</li>\n";
            body << "<li>Req. Number #" << session.get<int>("req_no") << "</li>\n";
            body << "</ul>\n";

            res->setContentType("text/html");
            res->setBody(body.str());
        } else {
            res->setStatusCode("400 BAD REQUEST");
            res->setContentType("text/html");
            res->setBody("<h1>400 BAD REQUEST</h1>\n");
        }

        // Write Response
        session.write(res);
    }

    void messageSent(Nwg::Session &session, Nwg::MessageBuffer &msg)
    {
        session.close();
        _printf("|||| RESPONSE SENT #%d\n", session.get<int>("req_no"));
    }

    void sessionClosed(Nwg::Session &session)
    {
        _printf("|||| SESSION CLOSED #%d\n", session.get<int>("req_no"));
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////
void run(int port)
{
    Nwg::EventLoop eventLoop;
    Nwg::Acceptor acceptor(&eventLoop, port);

    acceptor.setBuffSize(BUFFSIZE);
    acceptor.setReadBuffSize(READBUFFSIZE);
    acceptor.setProtocolCodec(std::make_shared<HttpProtocolCodec>());
    acceptor.setHandler(std::make_shared<HttpHandler>());

    printf("Listening on port %d\n", acceptor.getPort());
    printf("Open http://127.0.0.1:%d/\n", acceptor.getPort());
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
