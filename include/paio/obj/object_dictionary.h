#pragma once

#include <map>
//#include <paio/paio.h>
#include <string>
#include <paio/obj/object.h>

namespace paio {
  /* 
  struct ObjectContainer {
  private:
    std::shared_ptr<void> object;

    std::string typeName;
  public:
    ObjectContainer() : object(nullptr) {}

    template<typename T>
    ObjectContainer(std::shared_ptr<T> o) {
      object = std::static_pointer_cast<void>(o);
      typeName = typeid(T).name();
    }

    ObjectContainer(ObjectContainer&& obj) : object(std::move(obj.object)), typeName(obj.typeName) {
    }

    ObjectContainer& operator=(ObjectContainer&& obj) {
      object = std::move(obj.object);
      typeName = obj.typeName;
      return *this;
    }

    ObjectContainer(const ObjectContainer& obj): object(obj.object), typeName(obj.typeName) {}

    ObjectContainer& operator=(const ObjectContainer& obj) {
      object = obj.object;
      typeName = obj.typeName;
      return *this;
    }

    ~ObjectContainer() {}

  public:
    template<typename T>
    std::shared_ptr<T> get() { return std::static_pointer_cast<T>(object); }
  }; 
  */

  class ObjectContainer : public ObjectBase<std::shared_ptr<void>> {
  public:
  };

  struct ObjectDictionary {
  public:
    std::map<std::string, ObjectContainer> map;
  };

  using ObjectDictionary_ptr = std::shared_ptr<ObjectDictionary>;

  ObjectDictionary_ptr object_dictionary();

  template<typename T>
  paio::Object<T> get(ObjectDictionary_ptr dic, const std::string& key) {
    if (dic->map.count(key) == 0) {
      return paio::Object<T>();
    }
    return Object<T>(dic->map[key].get());
  }
  
  template<typename T>
  void put(ObjectDictionary_ptr dic, const std::string& key, paio::Object<T>&& doc) {
    dic->map[key] = ObjectContainer(doc._get());
  }

  template<typename T>
  void put(ObjectDictionary_ptr dic, const std::string& key, const paio::Object<T>& doc) {
    dic->map[key] = ObjectContainer(doc.get());
  }

  inline void forEach(ObjectDictionary_ptr dic, void(*f)(const std::string&, paio::ObjectContainer&)) {
    for (auto& [key, value] : dic->map){
        f(key, value);
    }
  }
};
