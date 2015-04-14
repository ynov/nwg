#include "nwg_session.h"

namespace Nwg
{

Session::Session(int bufferAllocationSize)
    : _writeObject(nullptr),
      _readBuffer(ByteBuffer(bufferAllocationSize)),
      _writeBuffer(ByteBuffer(bufferAllocationSize))
{
}

Session::~Session()
{
}

void Session::write(std::shared_ptr<Object> obj)
{
    _writeObject = obj;
}

Object &Session::getWriteObject()
{
    return *_writeObject;
}

ByteBuffer &Session::getReadBuffer()
{
    return _readBuffer;
}

ByteBuffer &Session::getWriteBuffer()
{
    return _writeBuffer;
}

} /* namespace Nwg */
