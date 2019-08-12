#pragma once

#include <string>
#include <map>
#include <memory>
#include "paio/obj/mangling.h"
#include "paio/obj/object_base.h"

namespace paio
{

class ObjectContainer : public ObjectBase<std::shared_ptr<void>>
{
public:
  std::string typeName;

  ObjectContainer() : ObjectBase<std::shared_ptr<void>>(nullptr), typeName("void") {}

  ObjectContainer(const std::shared_ptr<void> &&v, const std::string &&typeName) : ObjectBase<std::shared_ptr<void>>(std::move(v)), typeName(std::move(typeName)) {}

  ObjectContainer(ObjectContainer &&oc) : ObjectContainer(std::move(oc._privateData), std::move(oc.typeName)) {}

  ObjectContainer &operator=(ObjectContainer &&oc)
  {
    this->_privateData = std::move(oc._privateData);
    this->typeName = std::move(oc.typeName);
    return *this;
  }

  template <typename T>
  ObjectContainer(const std::shared_ptr<T> data) : ObjectBase<std::shared_ptr<void>>(std::static_pointer_cast<void>(data)), typeName(paio::demangle_name(typeid(T).name())) {}

  template <typename T>
  ObjectContainer(const T &data) : ObjectContainer(std::make_shared<T>(data)) {}

public:
  template <typename T, typename W>
  std::optional<W> bind(std::function<W(const T &)> f)
  {
    return _privateData ? f(*(std::static_pointer_cast<const T>(this->get()))) : std::nullopt;
  }
};

template<typename T>
inline ObjectContainer object_container(T&& t) {
    return ObjectContainer(std::make_shared<T>(t));
}

inline bool isNull(const ObjectContainer &oc)
{
  return !(oc.get());
}

template <typename T>
T get(const ObjectContainer &oc)
{
  return *(std::static_pointer_cast<const T>(oc.get()));
}

}; // namespace paio