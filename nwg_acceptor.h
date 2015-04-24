#ifndef NWG_ACCEPTOR_H_
#define NWG_ACCEPTOR_H_

#include "nwg_service.h"

namespace Nwg
{

struct ListenerEventArg;
class ProtocolCodec;

class Acceptor : public Service
{
public:
    Acceptor(int port, const Acceptor *acceptor = nullptr);
    virtual ~Acceptor();

    void setProtocolCodec(const std::shared_ptr<ProtocolCodec> &protocolCodec);
    void setHandler(const std::shared_ptr<Handler> &handler);

    Handler &getHandler();
    ProtocolCodec &getProtocolCodec();

    void setPort(int port);
    void setBuffSize(int buffsize);
    void setReadBuffSize(int buffsize);

    int getPort();
    size_t getBuffSize();
    size_t getReadBuffSize();

    struct event_base *getBase() const;

    void listen();

private:
    int _port;
    struct event_base *_base;

    evutil_socket_t _listenerFd;
    struct event *_listenerEvent;
    ListenerEventArg *_listenerEventArg;

    std::shared_ptr<ProtocolCodec> _protocolCodec;
    std::shared_ptr<Handler> _handler;

    size_t _buffSize = DEFAULT_BUFFSIZE;
    size_t _readBuffSize = DEFAULT_READBUFFSIZE;
};

struct ListenerEventArg
{
    struct event_base *base;
    Acceptor *acceptor;
};

} /* namespace Nwg */

#endif /* NWG_ACCEPTOR_H_ */
