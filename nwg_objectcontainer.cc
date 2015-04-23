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

void ObjectContainer::setObject(std::unique_ptr<Object> obj)
{
    _obj = std::move(obj);
}

Object &ObjectContainer::getObject()
{
    return *_obj;
}

} /* namespace Nwg */
