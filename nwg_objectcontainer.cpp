/*
 * nwg_objectcontainer.cpp
 *
 *  Created on: Apr 14, 2015
 *      Author: inov
 */

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

void ObjectContainer::setObject(std::shared_ptr<Object> obj)
{
    _obj = obj;
}

std::shared_ptr<Object> ObjectContainer::object()
{
    return _obj;
}

} /* namespace Nwg */
