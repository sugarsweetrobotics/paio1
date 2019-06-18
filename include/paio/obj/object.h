#pragma once
#include <optional>

namespace paio
{

class ObjectBase
{
public:
  ObjectBase() {}
  virtual ~ObjectBase() {}

public:
};

extern ObjectBase nullobj;

template <typename T>
class Object : public ObjectBase
{
private:
  std::shared_ptr<T> _privateData;

public:
  static Object<T> nullobj;
  Object()
  {
    _privateData = nullptr; //new T();
  }

  virtual ~Object()
  {
    //delete _privateData;
  }

public:
  Object(const Object &object) : Object()
  {
    *this = object;
  }

  Object &operator=(const Object &obj)
  {

    if (!_privateData)
    {
      _privateData = nullptr;
    }
    else
    {

      *_privateData = *(obj._privateData);
    }
    return *this;
  }

  Object(const T &privateData)
  {
    //_privateData = std::shared_ptr<T>(new T(privateData));
    _privateData = std::shared_ptr<T>(new T(privateData));
  }

  Object(std::shared_ptr<T> privateData) : _privateData(privateData)
  {
  }

  Object(T &&privateData)
  {
    _privateData = std::shared_ptr<T>(new T(std::forward<T>(privateData)));
  }

  Object(Object &&object)
  {
    this->_privateData = object._privateData;
    object._privateData = nullptr;
  }

  Object &operator=(Object &&obj)
  {
    // delete this->_privateData;
    this->_privateData = obj._privateData;
    obj._privateData = nullptr;
    return *this;
  }

public:
  const std::shared_ptr<T> get() const { return _privateData; }

  std::shared_ptr<T> _get() { return _privateData; }

public:
  Object<T> &does()
  {
    return *this;
  }

  template <typename... P>
  Object<T> &does(std::function<T(T &&)> f, P... rem)
  {
    if(_privateData) {
      *_privateData = std::forward<T>(f(std::move(*_privateData)));
    }
    return does(rem...);
  }

  template <typename P>
  P operator>>=(std::function<P(const T &)> f)
  {
    return std::move(f(*(this->_get())));
  }

  template <typename P>
  std::optional<P> operator>>=(P (*f)(const T &))
  {
    if (!_privateData) {
    return std::nullopt;
    }
      return std::move(f(*(this->_get())));
  }

  template <typename P>
  std::optional<P> bind(std::function<P(const T &)> f)
  {
    return (*this) >>= f;
  }

public:
  template <typename P>
  friend bool operator==(const Object<P> &o1, const Object<P> &o2);
};

template <typename T>
bool operator==(const Object<T> &o1, const Object<T> &o2)
{
  return (*(o1._privateData)) == (*(o2._privateData));
}

template <typename T>
std::shared_ptr<T> get(Object<T> &obj)
{
  return obj._get();
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
  return Object<T>(std::move(privateData));
}

template <typename T>
bool isNull(const Object<T> &obj)
{
  return !obj.get();
}
}; // namespace paio