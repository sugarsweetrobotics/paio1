#pragma once
#include <iostream>
#include <optional>

#include <paio/obj/object_base.h>

namespace paio
{

template <typename T>
class Object : public ObjectBase<std::shared_ptr<T>>
{
public:
  Object() : ObjectBase<std::shared_ptr<T>>(nullptr) {}

  virtual ~Object() {}

  explicit Object(std::shared_ptr<T> privateData) : ObjectBase<std::shared_ptr<T>>(privateData) {}

  explicit Object(const T &privateData) : Object<T>(std::make_shared<T>(privateData)) {}

  explicit Object(T &&privateData) : Object<T>(std::make_shared<T>(std::forward<T>(privateData))) {}

  explicit Object(T* privateData) : Object<T>(std::shared_ptr<T>(privateData)) {}

  Object(const Object &object) : Object<T>(object.get()) {}

  Object &operator=(const Object &obj) 
  {
    if (obj._privateData) {
      this->_privateData = std::make_shared<T>(*(obj.get()));
    } else {
      this->_privateData = nullptr;
    }
    return *this;
  }

  template <typename R>
  friend bool operator==(const Object<R> &o1, const Object<R> &o2);

public:
  template <typename P>
  P operator>>=(std::function<P(const T &)> f)
  {
    if (!this->_privateData) {
      throw NullObjectIsBindedError();
    }
    return f(*(this->get()));
  }

  template <typename P>
  P operator>>=(P (*f)(const T &)) const
  {
    if (!this->_privateData) {
      throw NullObjectIsBindedError();
    }
    return f(*(this->get()));
  }

  template <typename P>
  std::optional<P> bind(std::function<P(const T &)> f) const
  {
    if (this->_privateData) {
      return f(*(this->get()));
    }
    return std::nullopt;
  }

  Object<T> &does()
  {
    return *this;
  }

  template <typename... P>
  Object<T> &does(std::function<T(T &&)> f, P... rem)
  {
    if (this->_privateData)
    {
      *(this->_privateData) = std::forward<T>(f(std::move(*(this->get()))));
    }
    return does(rem...);
  }

  template <typename P>
  friend std::shared_ptr<P> get(Object<P> &obj);
}; /// Object


template <typename T>
bool isNull(const Object<T> &obj)
{
  return !obj.get();
}


template <typename T>
bool operator==(const Object<T> &o1, const Object<T> &o2)
{
  if (paio::isNull(o1)) {
    return paio::isNull(o2);
  }
  return (*(o1._privateData)) == (*(o2._privateData));
}

template <typename T>
Object<T> retn(const T &privateData)
{
  return Object<T>(privateData);
}

template <typename T>
Object<T> object(const T &privateData)
{
  return Object<T>(privateData);
}

template <typename T>
Object<T> object(T &&privateData)
{
  return Object<T>(std::forward<T>(privateData));
}

template <typename T>
std::shared_ptr<T> get(Object<T> &obj)
{
  return obj._get();
}

}; // namespace paio