#ifndef NWG_MESSAGEBUFFER_H_
#define NWG_MESSAGEBUFFER_H_

#include <vector>
#include <string>

typedef unsigned char byte;

namespace Nwg
{

class MessageBuffer
{
public:
    MessageBuffer();
    MessageBuffer(int size);
    virtual ~MessageBuffer();
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
    void read(std::vector<byte> &bs, int size);
    std::vector<byte> read(int size);
    std::vector<byte> readUntil(byte mark);
    void readUntil(std::vector<byte> &bs, byte mark);
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
