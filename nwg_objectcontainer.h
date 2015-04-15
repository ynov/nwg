#ifndef NWG_OBJECTCONTAINER_H_
#define NWG_OBJECTCONTAINER_H_

#include <memory>

#include "nwg_object.h"

namespace Nwg
{

class ObjectContainer
{
public:
    ObjectContainer();
    virtual ~ObjectContainer();

    void setObject(Object *obj);
    Object &getObject();

private:
    std::shared_ptr<Object> _obj;
};

} /* namespace Nwg */

#endif /* NWG_OBJECTCONTAINER_H_ */
