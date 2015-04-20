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
void ByteBuffer::reset() { _limit = 0; _position = 0; }
void ByteBuffer::flip() { _limit = _position; _position = 0; }
int ByteBuffer::remaining() { return _limit - _position; }
size_t ByteBuffer::position() { return _position; }
size_t ByteBuffer::limit() { return _limit; }

void ByteBuffer::put(byte b)
{
    _bs.insert(_bs.begin() + _position, b);
    _position++;
}

void ByteBuffer::put(const char *b, int size)
{
    _bs.insert(_bs.begin() + _position, b, b + size);
    _position += size;
}

void ByteBuffer::put(const byte *b, int size)
{
    _bs.insert(_bs.begin() + _position, b, b + size);
    _position += size;
}

void ByteBuffer::put(const std::vector<byte> &bs)
{
    _bs.insert(_bs.begin() + _position, bs.begin(), bs.end());
    _position += bs.size();
}

void ByteBuffer::put(const std::string &s)
{
    _bs.insert(_bs.begin() + _position, s.begin(), s.end());
    _position += s.length();
}

void ByteBuffer::put(int i)
{
    _bs.insert(_bs.begin() + _position, (byte *) &i, ((byte *) &i) + sizeof(int));
    _position += sizeof(int);
}

void ByteBuffer::put(float f)
{
    _bs.insert(_bs.begin() + _position, (byte *) &f, ((byte *) &f) + sizeof(float));
    _position += sizeof(float);
}

void ByteBuffer::put(double d)
{
    _bs.insert(_bs.begin() + _position, (byte *) &d, ((byte *) &d) + sizeof(double));
    _position += sizeof(double);
}

byte ByteBuffer::read()
{
    return _bs[_position++];
}

void ByteBuffer::read(byte *dest, int size)
{
    memcpy(dest, _bs.data() + _position, size);
    _position += size;
}

std::vector<byte> ByteBuffer::read(int size)
{
    std::vector<byte> bs(_bs.begin() + _position, _bs.begin() + _position + size);
    _position += size;

    return bs;
}

std::vector<byte> ByteBuffer::readUntil(byte mark)
{
    std::vector<byte> bs;
    bs.reserve(4096);

    byte b = read();
    while (b != mark && _position < _limit) {
        bs.push_back(b);
        b = read();
    }

    return bs;
}

std::string ByteBuffer::sread(int length)
{
    std::string s(_bs.begin() + _position, _bs.begin() + _position + length);
    _position += length;

    return s;
}

std::string ByteBuffer::sreadUntil(byte mark)
{
    std::string s;
    s.reserve(4096);

    byte b = read();
    while (b != mark && _position < _limit) {
        s.push_back(b);
        b = read();
    }

    return s;
}

int ByteBuffer::readInt()
{
    int i =  *((int *) &(_bs[_position]));
    _position += sizeof(int);

    return i;
}

float ByteBuffer::readFloat()
{
    float f =  *((float *) &(_bs[_position]));
    _position += sizeof(float);

    return f;
}

double ByteBuffer::readDouble()
{
    double d =  *((double *) &(_bs[_position]));
    _position += sizeof(double);

    return d;
}

byte &ByteBuffer::operator[](int i)
{
    return _bs[i];
}

std::vector<byte> &ByteBuffer::bsRef()
{
    return _bs;
}

} /* namespace Nwg */
