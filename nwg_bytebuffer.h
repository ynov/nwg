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
    int position();
    int limit();

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
    std::string getString(int length);

    void read(byte *b, int size);
    std::vector<byte> read(int size);
    std::string sread(int length);

    std::vector<byte> readUntil(byte mark);
    std::string sreadUntil(byte mark);

    int getInt();
    float getFloat();
    double getDouble();

    byte &operator[](int i);

private:
    int _position;
    int _limit;
    std::vector<byte> _bs;
};

} /* namespace Nwg */

#endif /* NWG_BYTEBUFFER_H_ */
