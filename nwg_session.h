#ifndef NWG_SESSION_H_
#define NWG_SESSION_H_

#include <memory>

#include "nwg_object.h"
#include "nwg_bytebuffer.h"

namespace Nwg
{

class Session: public Object
{
public:
    Session(int bufferAllocationSize);
    virtual ~Session();

    void write(std::shared_ptr<Object> obj);

    Object &getWriteObject();
    ByteBuffer &getReadBuffer();
    ByteBuffer &getWriteBuffer();

private:
    std::shared_ptr<Object> _writeObject;

    ByteBuffer _readBuffer;
    ByteBuffer _writeBuffer;
};

} /* namespace Nwg */

#endif /* NWG_SESSION_H_ */
