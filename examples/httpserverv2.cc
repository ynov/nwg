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
#define SILENT 1
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
static path workingPath;

struct SessionState : public Nwg::Object {
    int reqNo             = 0;
    size_t nwritten       = 0;
    size_t length         = 0;
    std::ifstream *is     = nullptr;
    bool readAndWriteFile = false;
};

#define STATE "STATE"
#define GETMSG(obj) dynamic_cast<Nwg::ByteBuffer &>(obj)
#define GETSTATE(session) session.get<SessionState>(STATE)

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
        std::shared_ptr<SessionState> state(new SessionState());
        state->reqNo = ++numReq;

        session.put<SessionState>(STATE, state);
    }

    void messageReceived(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = GETMSG(obj);
        SessionState &state  = GETSTATE(session);

        if (session.stillReading) {
            session.close();
        }

        std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer(BUFFSIZE));

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

        path urlPath = workingPath;
        urlPath += url;

        /* request is okay and it is a directory */
        if (requestOk && is_directory(urlPath) && ((int) url.find("..") == -1))
        {
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
                        sout += "  <li><a href=\"" + it->filename().string() + "/\">" + it->filename().string() + "/</li>\n";
                    } else {
                        sout += "  <li><a href=\"" + it->filename().string() + "\">" + it->filename().string() + "</li>\n";
                    }
                }
            }

            out->put("HTTP/1.1 200 OK\r\n");
            out->put("Content-Type: text/html\r\n");
            out->put("Connection: close\r\n");
            out->put("\r\n");

            out->put("<h1>Listing of " + urlPath.string() + "</h1>\n");
            out->put("<ul>\n");
            out->put(sout);
            out->put("</ul>\n");

            out->flip();

            session.write(out);
            return;
        }

        /* request is okay and it is likely a regular file */
        else if (requestOk)
        {
            url = url.substr(1);
            path p(url);

            if (is_regular_file(p))
            {
                std::ifstream *is = new std::ifstream(p.string(), std::ios::binary);
                is->seekg(0, is->end);
                int length = is->tellg();
                is->seekg(0, is->beg);

                state.readAndWriteFile = true;
                state.length = length;
                state.nwritten = 0;
                state.is = is;

                _printf("==== DATA TO BE WRITTEN: %d ====\n", length);

                out->put("HTTP/1.1 200 OK\r\n");
                out->put("Content-Type: text/plain\r\n");
                out->put("Connection: close\r\n");
                out->put("Content-Length: " + std::to_string(length) + "\r\n");
                out->put("\r\n");

                out->flip();

                /* send the header first */
                /* will continue reading the file and sending it at messageSent() */

                session.write(out);
                return;
            }

            /* neither regular file nor directory, let's assume it is not found */
            else
            {
                out->put("HTTP/1.1 404 NOT FOUND\r\n");
                out->put("Content-Type: text/html\r\n");
                out->put("Connection: close\r\n");
                out->put("\r\n");

                out->put("<h1>404 NOT FOUND</h1>\n");
                out->put("<pre>" + url + "</pre>\n");

                out->flip();

                session.write(out);
                return;
            }
        }

        /* request is NOT okay */
        else
        {
            out->put("HTTP/1.1 400 BAD REQUEST\r\n");
            out->put("Content-Type: text/html\r\n");
            out->put("Connection: close\r\n");
            out->put("\r\n");

            out->put("<h1>400 BAD REQUEST</h1>\n");

            out->flip();

            session.write(out);
            return;
        }
    }

    void messageSent(Nwg::Session &session, Nwg::Object &obj)
    {
        Nwg::ByteBuffer &msg = GETMSG(obj);
        SessionState &state  = GETSTATE(session);

        if (state.readAndWriteFile && state.nwritten < state.length)
        {
            /* continue reading file and sending it chunk by chunk @READBUFFSIZE */

            std::shared_ptr<Nwg::ByteBuffer> out(new Nwg::ByteBuffer(BUFFSIZE));

            std::ifstream *is = state.is;
            char *buff = new char[READBUFFSIZE];

            is->read(buff, READBUFFSIZE);
            out->put(buff, is->gcount());
            out->flip();

            delete [] buff;

            state.nwritten += is->gcount();
            session.write(out);

            if (state.nwritten == state.length) {
                state.is->close();
                delete state.is;
                state.is = nullptr;

                state.readAndWriteFile = false;
                state.length = 0;
                state.nwritten = 0;

                _printf("==== STREAM CLOSED ====\n");
            }
        }

        /* there is nothing left to be written */
        else
        {
            session.close();

            _printf("==== DATA IS COMPLETELY SENT ====\n");
        }
    }

    void sessionClosed(Nwg::Session &session)
    {
        SessionState &state = GETSTATE(session);

        if (state.readAndWriteFile) {
            state.is->close();
            delete state.is;
            state.is = nullptr;

            _printf("==== STREAM CLOSED ====\n");
            _printf("==== DATA IS NOT COMPLETELY SENT ====\n");
        }
    }
};

boost::regex HttpHandler::pattern = boost::regex(PATTERN);

void run(int port)
{
    Nwg::EventLoop eventLoop;
    Nwg::Acceptor acceptor(port, &eventLoop);

    acceptor.setBuffSize(BUFFSIZE);
    acceptor.setReadBuffSize(READBUFFSIZE);
    acceptor.setProtocolCodec(std::make_shared<Nwg::BasicProtocolCodec>());
    acceptor.setHandler(std::make_shared<HttpHandler>());

    printf("Listening on port %d\n", acceptor.getPort());
    printf("Open http://127.0.0.1:%d/\n", acceptor.getPort());
    acceptor.listen();

    eventLoop.dispatch();
}

int main(int argc, char **argv)
{
    workingPath = absolute(current_path());

    int port = [=]() {
        if (argc > 1) {
            return std::stoi(argv[1]);
        } else {
            return 8840;
        }
    }();

    run(port);
    return 0;
}
