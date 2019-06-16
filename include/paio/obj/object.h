#pragma once
#include <optional>

namespace paio {


  class ObjectBase {
  public:
    virtual ~ObjectBase() {}
  public:
  };

  template<typename T>
    class Object : public ObjectBase {
    private:
      T* _privateData;

    public:
      Object() {
	_privateData = new T();
      }

      virtual ~Object() {
	delete _privateData;
      }

    public:
    Object(const Object& object) : Object() {
      *this = object;
    }
      
      Object& operator=(const Object& obj) {
	*_privateData = *(obj->_privateData);
      }
	
	Object(const T& privateData) {
	  _privateData = new T(privateData);
	}

	Object(T&& privateData) {
	  _privateData = new T(std::forward<T>(privateData));
	}

      Object(Object&& object) {
	this->_privateData = object._privateData;
	object._privateData = nullptr;
      }
      
      Object& operator=(Object&& obj) {
	delete this->_privateData;
	this->_privateData = obj._privateData;
	obj._privateData = nullptr;
      }
	
	  
    public:

      T* _get() { return _privateData; }

    public:
      Object<T>& does() {
	return *this;
      }
 
      template<typename... P>
      Object<T>& does(std::function<T(T&&)> f, P... rem) {
	*_privateData = std::forward<T>(f(std::move(*_privateData)));
	return does(rem...);
      }

      template<typename P>
      P operator>>=(std::function<P(const T&)> f) {
	return std::move(f(*(this->_get())));
	}

      template<typename P>
      P operator>>=(P(*f)(const T&)) {
	return std::move(f(*(this->_get())));
      }

      template<typename P>
      P bind(std::function<P(const T&)> f) {
	return (*this)>>=f;
      }

    public:
      template<typename P>
      friend bool operator==(const Object<P>& o1, const Object<P>& o2);
    };

  template<typename T> bool operator==(const Object<T>& o1, const Object<T>& o2) {
    return (*(o1._privateData)) == (*(o2._privateData));
  }

  template<typename T> T* get(Object<T>& obj) {
    return obj._get();
  }

  template<typename T> Object<T> retn(const T& privateData) {
    return Object<T>(privateData);
  }

  template<typename T> Object<T> object(const T& privateData) {
    return Object<T>(privateData);
  }

  template<typename T> Object<T> object(T&& privateData) {
    return Object<T>(std::move(privateData));
  }
};

