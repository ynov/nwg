#include "nwg_bytebuffer.h"

#include <cstring>

namespace Nwg
{

ByteBuffer::ByteBuffer()
    : _position(0), _limit(0) {}

ByteBuffer::ByteBuffer(int size)
    : _position(0), _limit(0)
{
    _bs.reserve(size);
}

ByteBuffer::~ByteBuffer() {}
int ByteBuffer::ssize() { return _limit; }

void ByteBuffer::forward(int n) { _position += n; }
void ByteBuffer::rewind(int n) { _position -= n; }
void ByteBuffer::jump(int n) { _position = n; }
void ByteBuffer::flip() { _limit = _position; _position = 0; }
int ByteBuffer::remaining() { return _limit - _position; }
int ByteBuffer::position() { return _position; }
int ByteBuffer::limit() { return _limit; }

void ByteBuffer::putByte(byte b)
{
    _bs.insert(_bs.begin() + _position, b);
    _position++;
}

void ByteBuffer::putBytes(const byte *b, int size)
{
    _bs.insert(_bs.begin() + _position, b, b + size);
    _position += size;
}

void ByteBuffer::putBytes(const char *b, int size) { return putBytes((const byte *) b, size); }

void ByteBuffer::putBytes(const std::vector<byte> &bs)
{
    _bs.insert(_bs.begin() + _position, bs.begin(), bs.end());
    _position += bs.size();
}

void ByteBuffer::putString(const std::string &s)
{
    _bs.insert(_bs.begin() + _position, s.begin(), s.end());
    _position += s.length();
}

void ByteBuffer::putInt(int i)
{
    _bs.insert(_bs.begin() + _position, (byte *) &i, ((byte *) &i) + sizeof(int));
    _position += sizeof(int);
}

void ByteBuffer::putFloat(float f)
{
    _bs.insert(_bs.begin() + _position, (byte *) &f, ((byte *) &f) + sizeof(float));
    _position += sizeof(float);
}

void ByteBuffer::putDouble(double d)
{
    _bs.insert(_bs.begin() + _position, (byte *) &d, ((byte *) &d) + sizeof(double));
    _position += sizeof(double);
}

byte ByteBuffer::getByte()
{
    return _bs[_position++];
}

void ByteBuffer::getBytes(byte *dest, int size)
{
    memcpy(dest, _bs.data() + _position, size);
    _position += size;
}

std::vector<byte> ByteBuffer::getBytes(int size)
{
    std::vector<byte> bs(_bs.begin() + _position, _bs.begin() + _position + size);
    _position += size;

    return bs;
}

void ByteBuffer::read(byte *dest, int size) { getBytes(dest, size); }
std::vector<byte> ByteBuffer::read(int size) { return getBytes(size); }

std::string ByteBuffer::getString(int length)
{
    std::string s(_bs.begin() + _position, _bs.begin() + _position + length);
    _position += length;

    return s;
}

int ByteBuffer::getInt()
{
    int i =  *((int *) &(_bs[_position]));
    _position += sizeof(int);

    return i;
}

float ByteBuffer::getFloat()
{
    float f =  *((float *) &(_bs[_position]));
    _position += sizeof(float);

    return f;
}

double ByteBuffer::getDouble()
{
    double d =  *((double *) &(_bs[_position]));
    _position += sizeof(double);

    return d;
}

byte &ByteBuffer::operator[](int i)
{
    return _bs[i];
}

} /* namespace Nwg */
