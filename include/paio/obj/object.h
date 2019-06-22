#pragma once
#include <optional>

namespace paio
{

template <typename T>
class ObjectBase 
{
protected:
  T _privateData;

public:
  static ObjectBase<T> nullobj;

public:
  ObjectBase()
  {
     _privateData = nullptr; //new T();
  }

  virtual ~ObjectBase()
  {
    //delete _privateData;
  }

  ObjectBase(const ObjectBase &object) : ObjectBase()
  {
    *this = object;
  }

  ObjectBase &operator=(const ObjectBase &obj)
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


  ObjectBase(T privateData) : _privateData(privateData)
  {
  }

  ObjectBase(ObjectBase &&object)
  {
    this->_privateData = object._privateData;
    object._privateData = nullptr;
  }

  ObjectBase &operator=(ObjectBase &&obj)
  {
    // delete this->_privateData;
    this->_privateData = obj._privateData;
    obj._privateData = nullptr;
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
  friend bool operator==(const ObjectBase<P> &o1, const ObjectBase<P> &o2);

  template <typename P>
  friend std::shared_ptr<P> get(ObjectBase<P> &obj);

}; /// ObjectBase



template<typename T>
class Object : public ObjectBase<std::shared_ptr<T>> {

public:

public:
  Object() : ObjectBase<std::shared_ptr<T>>() {}

  virtual ~Object() {}
  /*  
  Object(const Object &object) : ObjectBase<std::shared_ptr<T>>(object) {}

  Object &operator=(const Object &obj) 
  {
    return this->operator=(obj);
  }

  Object(const T &privateData)  {
     *this->_privateData = privateData;
  }

  Object(T privateData) : ObjectBase<std::shared_ptr<T>>(privateData)
  {
  }

  Object(T &&privateData)  {
    this->_privateData = std::shared_ptr(std::move(privateData));
  }

  Object(Object &&object) : ObjectBase<std::shared_ptr<T>>(std::move(object)) {}

  Object &operator=(Object &&obj)
  {
    return this->operator=(std::move(obj));
  }
   */
  template <typename R>
  friend bool operator==(const Object<R> &o1, const Object<R> &o2);
  

public:
  template <typename P>
  std::optional<P> operator>>=(P(*f)(const T &)) 
  {
    if (!this->_privateData) {
    return std::nullopt;
    }
      return std::move(f(*(this->_get())));
  }

  template <typename P>
  std::optional<P> bind(std::function<P(const T &)> f)
  {
    return (*this) >>= f;
  }

  friend std::shared_ptr<T> get(Object<T> &obj);
 
};

template <typename T>
bool operator==(const Object<T> &o1, const Object<T> &o2)
{
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
  return Object<T>((privateData));
}

template <typename T>
bool isNull(const Object<T> &obj)
{
  return !obj.get();
}

template <typename T>
T get(ObjectBase<T> &obj)
{
  return obj._privateData;
}

template <typename T>
T* get(Object<T> &obj)
{
  return obj._privateData.get();
}



}; // namespace paio