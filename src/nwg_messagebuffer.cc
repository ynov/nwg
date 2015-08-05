#include "nwg_messagebuffer.h"

#include <cstring>

#define RESERVE_SIZE 1 << 16 /* 64KB */

namespace Nwg
{

MessageBuffer::MessageBuffer()
    : _position(0), _limit(0) {}

MessageBuffer::MessageBuffer(int size)
    : _position(0), _limit(0)
{
    _bs.reserve(size);
}

MessageBuffer::~MessageBuffer() {}
int MessageBuffer::ssize() { return _limit; }

void MessageBuffer::forward(int n) { _position += n; }
void MessageBuffer::rewind(int n) { _position -= n; }
void MessageBuffer::jump(int n) { _position = n; }
void MessageBuffer::reset() { _bs.clear(); _limit = 0; _position = 0; }
void MessageBuffer::flip() { _limit = _position; _position = 0; }
int MessageBuffer::remaining() { return _limit - _position; }
size_t MessageBuffer::position() { return _position; }
size_t MessageBuffer::limit() { return _limit; }

void MessageBuffer::put(byte b)
{
    _bs.insert(_bs.begin() + _position, b);
    _position++;
}

void MessageBuffer::put(const char *b, int size)
{
    _bs.insert(_bs.begin() + _position, b, b + size);
    _position += size;
}

void MessageBuffer::put(const byte *b, int size)
{
    _bs.insert(_bs.begin() + _position, b, b + size);
    _position += size;
}

void MessageBuffer::put(const std::vector<byte> &bs)
{
    _bs.insert(_bs.begin() + _position, bs.begin(), bs.end());
    _position += bs.size();
}

void MessageBuffer::put(const std::string &s)
{
    _bs.insert(_bs.begin() + _position, s.begin(), s.end());
    _position += s.length();
}

void MessageBuffer::put(int i)
{
    _bs.insert(_bs.begin() + _position, (byte *) &i, ((byte *) &i) + sizeof(int));
    _position += sizeof(int);
}

void MessageBuffer::put(float f)
{
    _bs.insert(_bs.begin() + _position, (byte *) &f, ((byte *) &f) + sizeof(float));
    _position += sizeof(float);
}

void MessageBuffer::put(double d)
{
    _bs.insert(_bs.begin() + _position, (byte *) &d, ((byte *) &d) + sizeof(double));
    _position += sizeof(double);
}

byte MessageBuffer::read()
{
    return _bs[_position++];
}

void MessageBuffer::read(byte *dest, int size)
{
    memcpy(dest, _bs.data() + _position, size);
    _position += size;
}

std::vector<byte> MessageBuffer::read(int size)
{
    std::vector<byte> bs(_bs.begin() + _position, _bs.begin() + _position + size);
    _position += size;

    return bs;
}

void MessageBuffer::read(std::vector<byte> &bs, int size)
{
    bs.insert(bs.begin(), _bs.begin() + _position, _bs.begin() + _position + size);
    _position += size;
}

void MessageBuffer::readUntil(std::vector<byte> &bs, byte mark)
{
    bs.reserve(RESERVE_SIZE);

    byte b = read();
    while (b != mark && _position < _limit) {
        bs.push_back(b);
        b = read();
    }
}

std::vector<byte> MessageBuffer::readUntil(byte mark)
{
    std::vector<byte> bs;
    readUntil(bs, mark);

    return bs;
}

std::string MessageBuffer::sread(int length)
{
    std::string s(_bs.begin() + _position, _bs.begin() + _position + length);
    _position += length;

    return s;
}

std::string MessageBuffer::sreadUntil(byte mark)
{
    std::string s;
    s.reserve(RESERVE_SIZE);

    byte b = read();
    while (b != mark && _position < _limit) {
        s.push_back(b);
        b = read();
    }

    return s;
}

int MessageBuffer::readInt()
{
    int i =  *((int *) &(_bs[_position]));
    _position += sizeof(int);

    return i;
}

float MessageBuffer::readFloat()
{
    float f =  *((float *) &(_bs[_position]));
    _position += sizeof(float);

    return f;
}

double MessageBuffer::readDouble()
{
    double d =  *((double *) &(_bs[_position]));
    _position += sizeof(double);

    return d;
}

byte &MessageBuffer::operator[](int i)
{
    return _bs[i];
}

std::vector<byte> &MessageBuffer::bsRef()
{
    return _bs;
}

} /* namespace Nwg */
