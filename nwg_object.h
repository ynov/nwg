#ifndef NWG_OBJECT_H_
#define NWG_OBJECT_H_

typedef unsigned char byte;

namespace Nwg
{

class Object
{
public:
    Object();
    virtual ~Object();
    virtual int ssize();
};

} /* namespace Nwg */

#endif /* NWG_OBJECT_H_ */
