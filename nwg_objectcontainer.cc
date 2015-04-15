#include "nwg_objectcontainer.h"

namespace Nwg
{

ObjectContainer::ObjectContainer()
    : _obj(nullptr)
{
}

ObjectContainer::~ObjectContainer()
{
    _obj.reset();
}

void ObjectContainer::setObject(Object *obj)
{
    _obj = std::shared_ptr<Object>(obj);
}

Object &ObjectContainer::getObject()
{
    return *_obj;
}

} /* namespace Nwg */
