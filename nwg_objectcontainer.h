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

    void setObject(std::shared_ptr<Object> obj);
    std::shared_ptr<Object> object();

private:
    std::shared_ptr<Object> _obj;
};

} /* namespace Nwg */

#endif /* NWG_OBJECTCONTAINER_H_ */
