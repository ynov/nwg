#ifndef NWG_SESSION_H_
#define NWG_SESSION_H_

#include <map>
#include <memory>
#include <functional>

#include <event2/event.h>

#include "nwg_object.h"
#include "nwg_bytebuffer.h"

namespace Nwg
{

class Service;

class Session
{
public:
    Session(int bufferAllocationSize);
    Session(int bufferAllocationSize,
            struct event_base *base,
            evutil_socket_t fd,
            Service *service);
    virtual ~Session();

    void write(const std::shared_ptr<Object> &obj);
    void close();

    int getBufferAllocationSize();

    bool isWriteObjectPresent();
    void resetWriteObject();
    void resetWrite();

    Object &getWriteObject();
    Object &getLastWriteObject();
    ByteBuffer &getReadBuffer();
    ByteBuffer &getWriteBuffer();
    Service &getService();

    bool isClosed();
    int getFd();

    bool stillReading = false;
    size_t nRead = 0;
    size_t nWritten = 0;
    struct event *readEvent = nullptr;
    struct event *writeEvent = nullptr;

    template<class T>
    void put(const std::string &key, const std::shared_ptr<T> &val)
    {
        _vars[key] = val;
    }

    template<class T>
    T &get(const std::string &key)
    {
        return *(T *) _vars[key].get();
    }

    template<class T>
    std::shared_ptr<T> &getPtr(const std::string &key)
    {
        return std::static_pointer_cast<T>(_vars[key]);
    }

private:
    int _bufferAllocationSize;
    std::shared_ptr<Object> _writeObject;
    std::shared_ptr<Object> _lastWriteObject;

    ByteBuffer _readBuffer;
    ByteBuffer _writeBuffer;

    evutil_socket_t _fd = -1;
    Service *_service = nullptr;

    bool _closed = false;
    std::map<std::string, std::shared_ptr<void>> _vars;
};

} /* namespace Nwg */

#endif /* NWG_SESSION_H_ */
