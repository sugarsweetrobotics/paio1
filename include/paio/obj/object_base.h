#pragma once

#include <exception>
#include <optional>


namespace paio {


class NullObjectIsBindedError : public std::exception {};

template <typename T>
class ObjectBase
{
public:
  using MemberType = T;
  using ActionType = std::function<T(T &&)>;
protected:
  T _privateData;
  
public:
  static ObjectBase<T> nullobj;

public:
  virtual ~ObjectBase() {}

  ObjectBase(const ObjectBase &object) : _privateData(object._privateData) {}

  ObjectBase &operator=(const ObjectBase &obj)
  {
    _privateData = (obj._privateData);
    return *this;
  }

  explicit ObjectBase(const T& privateData) : _privateData(privateData) {}

  ObjectBase(ObjectBase &&object) : _privateData(std::move(object._privateData)) {}

  ObjectBase &operator=(ObjectBase &&obj)
  {
    this->_privateData = std::move(obj._privateData);
    return *this;
  }

public:
  const T get() const { return _privateData; }

  T _get() { return _privateData; }

public:
  ObjectBase<T> &does()
  {
    return *this;
  }

  template <typename... P>
  ObjectBase<T> &does(std::function<T(T &&)> f, P... rem)
  {
    _privateData = std::forward<T>(f(std::move(_get())));
    return does(rem...);
  }

  template <typename P>
  P operator>>=(std::function<P(const T &)> f)
  {
    return std::move(f(*(_get())));
  }

  template <typename P>
  P operator>>=(P (*f)(const T &))
  {
    return std::move(f(*(_get())));
  }

  template <typename P>
  std::optional<P> bind(std::function<P(const T &)> f)
  {
    return (*this) >>= f;
  }

public:
  template <typename P>
  friend bool operator==(const ObjectBase<P> &o1, const ObjectBase<P> &o2);

  template <typename P>
  friend P get(ObjectBase<P> &obj);

}; /// ObjectBase


template <typename T>
bool operator==(const ObjectBase<T> &o1, const ObjectBase<T> &o2)
{
  return (*(o1._privateData)) == (*(o2._privateData));
}

template <typename T>
T get(ObjectBase<T> &obj)
{
  return obj._privateData;
}

}