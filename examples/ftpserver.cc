#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>

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
static path workingPath;

static int numReq = 0;
static int initialDataPort = 37100;

const std::string R_WELCOME_MSG      = "220 Nwg FTP Server Example\r\n";
const std::string R_SPECIFY_PASSWORD = "331 Please specify password.\r\n";
const std::string R_LOGIN_SUCCESSFUL = "230 Login successful.\r\n";
const std::string R_ENTER_PASV_      = "227 Entering Passive Mode ";
const std::string R_SHOW_PWD_        = "257 ";
const std::string R_OPENCONN_LIST    = "150 Here comes the directory listing.\r\n";
const std::string R_OPENCONN_RETR    = "150 Opening connection.\r\n";
const std::string R_TRF_COMPLETE     = "226 Transfer complete.\r\n";
const std::string R_DIR_CHANGED      = "250 Directory successfully changed.\r\n";
const std::string R_SYST             = "215 UNIX Type: L8\r\n";
const std::string R_MUST_LOGIN       = "530 You must login.\r\n";
const std::string R_UNKWN_COMMAND    = "550 Unknown command.\r\n";
const std::string R_GOODBYE          = "221 Goodbye.\r\n";
const std::string R_USE_PASV_FIRST   = "425 Use PASV first.\r\n";
const std::string R_TYPE             = "200 Switching to Binary mode.\r\n";

#define CMD_USER "USER"
#define CMD_PASS "PASS"
#define CMD_SYST "SYST"
#define CMD_PASV "PASV"
#define CMD_LIST "LIST"
#define CMD_NLST "NLST"
#define CMD_CWD  "CWD"
#define CMD_PWD  "PWD"
#define CMD_RETR "RETR"
#define CMD_QUIT "QUIT"
#define CMD_TYPE "TYPE"

std::vector<std::string> tokenize(const std::string &str)
{
    std::string buf;
    std::stringstream ss(str);
    std::vector<std::string> tokens;

    while (ss >> buf) {
        tokens.push_back(buf);
    }

    return tokens;
}

std::string port12(int port)
{
    std::stringstream ss;
    ss << port / 256 << "," << port % 256;

    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Ftp Data Handler
////////////////////////////////////////////////////////////////////////////////////////////////////
class FtpDataHandler : public Nwg::Handler
{
    struct State;

public:
    void setState(State *state) { _state = state; }
    void setCommand(const std::string &command) { _command = command; }
    void setParameter(const std::string &parameter) { _parameter = parameter; }
    void setCurrentPath(const path &currentPath) { _currentPath = currentPath; }

    void sessionOpened(Nwg::Session &session)
    {
        std::shared_ptr<Nwg::MessageBuffer> out(new Nwg::MessageBuffer(BUFFSIZE));
        out->put("");
        out->flip();
        session.write(out);
    }

    void messageReceived(Nwg::Session &session, Nwg::MessageBuffer &msg)
    {
    }

    void messageSent(Nwg::Session &session, Nwg::MessageBuffer &msg)
    {
        std::shared_ptr<Nwg::MessageBuffer> out(new Nwg::MessageBuffer(BUFFSIZE));

        if (closeNow) {
            session.close();
            _command = "";
            closeNow = false;
        }

        if (_command == "") {
            out->put("");
        }
        else {

            // LIST
            if (_command == CMD_LIST) {
                std::stringstream ss;
                path p = current_path() / _currentPath;

                if (is_directory(p)) {
                    typedef std::vector<path> vec;
                    vec v;

                    std::copy(directory_iterator(p), directory_iterator(), std::back_inserter<vec>(v));
                    sort(v.begin(), v.end());

                    for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it) {
                        if (is_directory(*it)) {
                            ss << "drwxrwxr-x    2 1000     1000 " << std::setw(10) << 4096 << " Aug 01 18:00 " << it->filename().string() << "\r\n";
                        } else {
                            ss << "-rw-rw-r--    1 1000     1000 " << std::setw(10) << file_size(_currentPath / it->filename()) << " Aug 01 18:00 " << it->filename().string() << "\r\n";
                        }
                    }
                }
                out->put(ss.str());
                closeNow = true;
            }

            // RETR
            else if (_command == CMD_RETR) {
                std::ifstream is((_currentPath.string() != "" ? _currentPath.string() + "/" : "") + _parameter, std::ios::binary);

                std::string content;

                is.seekg(0, std::ios::end);
                content.reserve(is.tellg());
                is.seekg(0, std::ios::beg);

                content.assign((std::istreambuf_iterator<char>(is)),
                                std::istreambuf_iterator<char>());

                out->put(content);
                closeNow = true;
            }

            else {
                out->put("Unknown command.\r\n");
                closeNow = true;
            }
        }

        out->flip();
        session.write(out);
    }

    void sessionClosed(Nwg::Session &session)
    {
    }

private:
    State *_state;
    path _currentPath;
    std::string _command = "";
    std::string _parameter = "";
    bool closeNow = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Ftp Command Handler
////////////////////////////////////////////////////////////////////////////////////////////////////
struct State
{
    int reqNo = -1;
    bool loggedIn = false;
    bool quit = false;

    bool pasvMode = false;
    int pasvPort = -1;

    path absolutePath;
    path relativePath;

    std::string lastCommand = "";
    std::string lastParameter = "";

    Nwg::Acceptor *dataHandlerAcceptor = NULL;
    std::shared_ptr<FtpDataHandler> dataHandler;
};

class FtpCommandHandler : public Nwg::Handler
{
public:
    void sessionOpened(Nwg::Session &session)
    {
        std::shared_ptr<Nwg::MessageBuffer> out(new Nwg::MessageBuffer(BUFFSIZE));

        std::shared_ptr<State> state(new State());
        state->reqNo = ++numReq;
        state->absolutePath = workingPath;
        state->relativePath = "";
        session.put<State>("state", state);

        printf("Session opened #%d\n", state->reqNo);

        out->put(R_WELCOME_MSG);
        out->flip();

        session.write(out);
    }

    void messageReceived(Nwg::Session &session, Nwg::MessageBuffer &msg)
    {
        std::shared_ptr<Nwg::MessageBuffer> out(new Nwg::MessageBuffer(BUFFSIZE));
        State &state = session.get<State>("state");

        std::vector<std::string> tokens = tokenize(msg.sreadUntil('\n'));
        std::string &command = tokens[0];

        printf("Command: %s\n", command.c_str());

        // Not logged in
        if (!state.loggedIn && command != CMD_QUIT) {
            if (state.lastCommand == CMD_USER) {
                if (command == CMD_PASS) {
                    state.loggedIn = true;
                    out->put(R_LOGIN_SUCCESSFUL);
                } else {
                    out->put(R_UNKWN_COMMAND);
                }
            } else {
                if (command == CMD_USER) {
                    state.lastCommand = CMD_USER;
                    out->put(R_SPECIFY_PASSWORD);
                } else {
                    out->put(R_MUST_LOGIN);
                }
            }

            goto end;
        }

        // PASV
        if (command == CMD_PASV) {
            state.pasvMode = true;
            if (state.pasvPort == -1) {
                state.pasvPort = initialDataPort++;
            }

            std::stringstream ss;
            ss << R_ENTER_PASV_ << "(127,0,0,1," << port12(state.pasvPort) << ")\r\n";
            out->put(ss.str());

            if (state.dataHandlerAcceptor == NULL) {
                state.dataHandler = std::make_shared<FtpDataHandler>();

                state.dataHandlerAcceptor = new Nwg::Acceptor(session.getService().getEventLoop(), state.pasvPort);
                state.dataHandlerAcceptor->setBuffSize(BUFFSIZE);
                state.dataHandlerAcceptor->setHandler(state.dataHandler);
                state.dataHandlerAcceptor->listen();
            }
        }

        // TYPE
        else if (command == CMD_TYPE) {
            out->put(R_TYPE);
        }

        // SYST
        else if (command == CMD_SYST) {
            out->put(R_SYST);
        }

        // CWD
        else if (command == CMD_CWD) {
            std::string npath = tokens[1];
            if (npath == ".." || npath == "../") {
                state.relativePath.remove_leaf();
            } else {
                state.relativePath /= path(npath);
            }
            out->put(R_DIR_CHANGED);
        }

        // PWD
        else if (command == CMD_PWD) {
            std::stringstream ss;
            ss << R_SHOW_PWD_ << "\"/" << state.relativePath.string() << "\"\r\n";
            out->put(ss.str());
        }

        // LIST
        else if (command == CMD_LIST) {
            if (!state.pasvMode) {
                out->put(R_USE_PASV_FIRST);
            } else {
                state.dataHandler->setCommand(CMD_LIST);
                state.dataHandler->setCurrentPath(state.relativePath);
                state.lastCommand = CMD_LIST;

                out->put(R_OPENCONN_LIST);
                out->put(R_TRF_COMPLETE);
                state.pasvMode = false;
            }
        }

        // RETR
        else if (command == CMD_RETR) {
            if (!state.pasvMode) {
                out->put(R_USE_PASV_FIRST);
            } else {
                state.dataHandler->setCommand(CMD_RETR);
                state.dataHandler->setParameter(tokens[1]);
                state.lastCommand = CMD_RETR;

                out->put(R_OPENCONN_RETR);
                out->put(R_TRF_COMPLETE);
                state.pasvMode = false;
            }
        }

        // QUIT
        else if (command == CMD_QUIT) {
            out->put(R_GOODBYE);
            state.quit = true;
        }

        // Unknown Command
        else {
            printf("GOT YA! %s\n", command.c_str());
            out->put(R_UNKWN_COMMAND);
        }

end:
        out->flip();
        session.write(out);
    }

    void messageSent(Nwg::Session &session, Nwg::MessageBuffer &msg)
    {
        State &state = session.get<State>("state");

        if (state.quit) {
            session.close();
            delete state.dataHandlerAcceptor;
        }
    }

    void sessionClosed(Nwg::Session &session)
    {
        State &state = session.get<State>("state");

        printf("Session closed #%d\n", state.reqNo);
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
    acceptor.setProtocolCodec(std::make_shared<Nwg::BasicProtocolCodec>());
    acceptor.setHandler(std::make_shared<FtpCommandHandler>());

    printf("Listening on port %d\n", acceptor.getPort());
    acceptor.listen();

    eventLoop.dispatch();
}

int main(int argc, char **argv)
{
    workingPath = absolute(current_path());

    run([&]() -> int {
        if (argc > 1) {
            return std::stoi(argv[1]);
        } else {
            return 8821;
        }
    }());
    return 0;
}
