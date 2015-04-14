#include "nwg_bytebuffer.h"

#include <cstdio>

int main(int argc, char **argv)
{
    printf("-- BEGIN --\n\n");

    Nwg::ByteBuffer b1(1024);
    Nwg::ByteBuffer b2(1024);
    Nwg::ByteBuffer b3(1024);

    Nwg::Object &obj = b3;

    b1.putString("abc");
    b1.putString("xyz");
    b1.flip();

    printf("abc = %s\n", b1.getString(3).c_str());
    printf("xyz = %s\n", b1.getString(3).c_str());
    printf("\n");

    b2.putString("foo");
    b2.putInt(66778899);
    b2.putDouble(3.1415);
    b2.putString("bar");
    b2.putFloat(4.55f);
    b2.putString("qux");
    b2.flip();

    // printf(" - pos: %d\n", b2.position());
    printf("foo = %s\n", b2.getString(3).c_str());
    printf("66778899 = %d\n", b2.getInt());
    printf("3.1415 = %.4lf\n", b2.getDouble());
    printf("bar = %s\n", b2.getString(3).c_str());
    printf("4.55 = %.2f\n", b2.getFloat());
    printf("qux = %s\n", b2.getString(3).c_str());
    printf("\n");

    do {
        Nwg::ByteBuffer &bbuf = dynamic_cast<Nwg::ByteBuffer &>(obj);

        byte bs[] = {
            0x01, 0x11,
            0x00, 0xf0
        };

        byte bt[4];

        // bbuf.putByte(bs[0]);
        // bbuf.putByte(bs[1]);
        // bbuf.putByte(bs[2]);
        // bbuf.putByte(bs[3]);

        // bbuf.putBytes(bs, sizeof(bs);

        std::vector<byte> bv(bs, bs + sizeof(bs));
        bbuf.putBytes(bv);

        bbuf.flip();

        bbuf.getBytes(bt, 4);

        printf("0x01 0x11 = 0x%02x 0x%02x\n", bt[0], bt[1]);
        printf("0x00 0xf0 = 0x%02x 0x%02x\n", bt[2], bt[3]);

        printf("bbuf.ssize() | 4 = %d\n", bbuf.ssize());
        printf("obj.ssize() | 4 = %d\n", obj.ssize());
    } while(0);

    printf("\n-- END --\n");
    return 0;
}
