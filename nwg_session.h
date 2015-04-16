#ifndef NWG_SESSION_H_
#define NWG_SESSION_H_

#include <map>
#include <memory>

#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

#include <event2/event.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>

#include "nwg_object.h"
#include "nwg_bytebuffer.h"

namespace Nwg
{

class Server;

class Session: public Object
{
public:
    Session(int bufferAllocationSize);
    Session(int bufferAllocationSize,
            struct event_base *base,
            evutil_socket_t fd,
            Server *server);
    virtual ~Session();

    void write(std::shared_ptr<Object> obj);
    void write(Object *obj);
    void close();

    int getBufferAllocationSize();

    bool isWriteObjectPresent();
    void resetWriteObject();
    void resetWrite();

    Object &getWriteObject();
    ByteBuffer &getReadBuffer();
    ByteBuffer &getWriteBuffer();
    Server &getServer();

    bool isClosed();
    int getFd();

    struct event *readEvent = nullptr;
    struct event *writeEvent = nullptr;

    template<class T>
    void put(const std::string &key, std::shared_ptr<T> val)
    {
        _vars[key] = val;
    }

    template<class T>
    void put(const std::string &key, T *val)
    {
        _vars[key] = std::shared_ptr<T>(val);
    }

    template<class T>
    T &get(const std::string &key)
    {
        return *(T *) _vars[key].get();
    }

private:
    int _bufferAllocationSize;
    std::shared_ptr<Object> _writeObject;

    ByteBuffer _readBuffer;
    ByteBuffer _writeBuffer;

    evutil_socket_t _fd = -1;
    Server *_server = nullptr;

    bool _closed = false;
    std::map<std::string, std::shared_ptr<void>> _vars;
};

} /* namespace Nwg */

#endif /* NWG_SESSION_H_ */
