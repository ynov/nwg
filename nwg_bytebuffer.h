#include "nwg_object.h"

#include <vector>
#include <string>

#ifndef NWG_BYTEBUFFER_H_
#define NWG_BYTEBUFFER_H_

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
    int remaining();
    int position();
    int limit();
    void flip();

    void putByte(byte b);
    void putBytes(const char *b, int size);
    void putBytes(const byte *b, int size);
    void putBytes(const std::vector<byte> &bs);

    void putString(const std::string &s);

    void putInt(int i);
    void putFloat(float f);
    void putDouble(double d);

    byte getByte();
    void getBytes(byte *dest, int size);
    std::vector<byte> getBytes(int size);

    void read(byte *b, int size);
    std::vector<byte> read(int size);

    std::string getString(int length);

    int getInt();
    float getFloat();
    double getDouble();

private:
    int _position;
    int _limit;
    std::vector<byte> _bs;
};

} // namespace Nwg

#endif
