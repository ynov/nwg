#include <cstdio>
#include <iostream>
#include <fstream>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include <nwg.h>
#include <nwg_basicprotocolcodec.h>

#define BUFFSIZE 32768

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

using namespace boost::filesystem;

static int numReq = 0;

struct SessionState : public Nwg::Object
{
    int reqNo = 0;
    bool done = false;
};

#define PATTERN "([a-zA-Z]+) (/[0-9a-zA-Z\\-_,.;:'\"\\[\\]\\(\\)+=!@#$%^&*<>/?~`{}|]*) (HTTP/\\d\\.\\d)(\r|)$"

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
        SessionState &state = *new SessionState();
        state.reqNo = ++numReq;

        session.put<SessionState>("_", &state);
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = dynamic_cast<Nwg::ByteBuffer &>(obj);
        SessionState &state = session.get<SessionState>("_");

        std::string h1 = msg.sreadUntil('\n');

        _printf("==== REQUEST RECEIVED %d ====\n", state.reqNo);
        _printf("%s\n", h1.c_str());

        smatch what;
        std::string url = "";
        bool requestOk = false;

        if (regex_match(h1, what, pattern)) {
            _printf("Method: %s\n", what[1].str().c_str());
            _printf("URL: %s\n", what[2].str().c_str());
            _printf("HTTP Version: %s\n", what[3].str().c_str());

            url = decodeUrl(what[2].str());
            requestOk = true;
        } else {
            _printf("MALFORMED REQUEST.\n");
        }

        _printf("==== ==== ====\n");

        Nwg::ByteBuffer &out = *new Nwg::ByteBuffer(BUFFSIZE);
        path urlPath = path(absolute(current_path()).string() + url);

        if (requestOk && is_directory(urlPath)) {
            std::string sout;
            sout.reserve(BUFFSIZE);

            path p = urlPath;
            if (is_directory(p)) {
                typedef std::vector<path> vec;
                vec v;

                std::copy(directory_iterator(p), directory_iterator(), std::back_inserter<vec>(v));
                sort(v.begin(), v.end());

                for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it) {
                    if (is_directory(*it)) {
                        sout += "  <li><a href=\"" + (*it).filename().string() + "/\">" + (*it).filename().string() + "/</li>\n";
                    } else {
                        sout += "  <li><a href=\"" + (*it).filename().string() + "\">" + (*it).filename().string() + "</li>\n";
                    }
                }
            }

            out.put("HTTP/1.1 200 OK\r\n");
            out.put("Content-Type: text/html\r\n");
            out.put("Connection: close\r\n");
            out.put("\r\n");

            out.put("<h1>Listing of " + urlPath.string() + "</h1>\n");
            out.put("<ul>\n");
            out.put(sout);
            out.put("</ul>\n");

            out.flip();

            session.write(&out);
            return;
        } /* end of url == "/" */
        else if (requestOk)
        {
            url = url.substr(1);
            path p(url);

            if (is_regular_file(p)) {
                std::ifstream is(p.string(), std::ios::binary);
                is.seekg (0, is.end);
                int length = is.tellg();
                is.seekg (0, is.beg);

                out.put("HTTP/1.1 200 OK\r\n");
                out.put("Content-Type: text/plain\r\n");
                out.put("Connection: close\r\n");
                out.put("Content-Length: " + std::to_string(length) + "\r\n");
                out.put("\r\n");

                char *buff = new char[length];
                is.read(buff, length);
                is.close();

                out.put(buff, length);
                delete []buff;

                out.flip();

                session.write(&out);
                return;
            }
            else
            {
                out.put("HTTP/1.1 404 NOT FOUND\r\n");
                out.put("Content-Type: text/html\r\n");
                out.put("Connection: close\r\n");
                out.put("\r\n");

                out.put("<h1>404 NOT FOUND</h1>\n");
                out.put("<pre>" + url + "</pre>\n");

                out.flip();

                session.write(&out);
                return;
            }
        } /* end of requestOk */
        else
        {
            out.put("HTTP/1.1 400 BAD REQUEST\r\n");
            out.put("Content-Type: text/html\r\n");
            out.put("Connection: close\r\n");
            out.put("\r\n");

            out.put("<h1>400 BAD REQUEST</h1>\n");

            out.flip();

            session.write(&out);
            return;
        }
    }

    void messageSent(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = dynamic_cast<Nwg::ByteBuffer &>(obj);
        SessionState &state = session.get<SessionState>("_");

        session.close();
    }

    void sessionClosed(Nwg::Session &session)
    {
        SessionState &state = session.get<SessionState>("_");
    }
};

boost::regex HttpHandler::pattern = boost::regex(PATTERN);

void run(int port)
{
    Nwg::Server server(port);
    server.setBuffSize(BUFFSIZE);

    server.setProtocolCodec(new Nwg::BasicProtocolCodec());
    server.setHandler(new HttpHandler());

    printf("Listening on port %d\n", server.getPort());
    printf("Open http://127.0.0.1:%d/\n", server.getPort());

    server.run();
}

int main(int argc, char **argv)
{
    int port = 8840;

    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    run(port);
    return 0;
}
