#ifndef NWG_BYTEBUFFER_H_
#define NWG_BYTEBUFFER_H_

#include <vector>
#include <string>

#include "nwg_object.h"

typedef unsigned char byte;

namespace Nwg
{

class ByteBuffer : public Object
{
public:
    ByteBuffer();
    ByteBuffer(int size);
    virtual ~ByteBuffer();
    virtual int ssize();

    void forward(int n);
    void rewind(int n);
    void jump(int n);
    void reset();
    void flip();
    int remaining();
    size_t position();
    size_t limit();

    void put(byte b);
    void put(const char *b, int size);
    void put(const byte *b, int size);
    void put(const std::vector<byte> &bs);
    void put(const std::string &s);

    void put(int i);
    void put(float f);
    void put(double d);

    byte read();
    void read(byte *b, int size);
    std::vector<byte> read(int size);
    std::vector<byte> readUntil(byte mark);
    std::string sread(int length);
    std::string sreadUntil(byte mark);

    int readInt();
    float readFloat();
    double readDouble();

    byte &operator[](int i);

    std::vector<byte> &bsRef();

private:
    size_t _position;
    size_t _limit;
    std::vector<byte> _bs;
};

} /* namespace Nwg */

#endif /* NWG_BYTEBUFFER_H_ */
