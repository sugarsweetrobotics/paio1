#pragma once

#include <map>
//#include <paio/paio.h>
#include <string>
#include <paio/obj/object.h>

namespace paio {

  class ObjectContainer : public ObjectBase<std::shared_ptr<void>> {
  public:

    std::string typeName;
    ObjectContainer() : ObjectBase<std::shared_ptr<void>>() , typeName("void") {
    }

    template<typename T>
    ObjectContainer(std::shared_ptr<T> data): ObjectBase<std::shared_ptr<void>>(std::static_pointer_cast<void>(data)){
      typeName = typeid(T).name();
    }
public:

    template<typename T, typename W>
    std::optional<W> bind(std::function<W(const T&)> f) {
      if (!this->_privateData) {
        return std::nullopt;
      }
      return f(*(std::static_pointer_cast<const T>(this->get())));
    }

  };

  template<typename T>
  T get(const ObjectContainer& oc) {
    return *(std::static_pointer_cast<const T>(oc.get()));
  }

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
    return paio::Object<T>(std::static_pointer_cast<T>(paio::get<std::shared_ptr<void>>(dic->map[key])));
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

  inline void forEach(ObjectDictionary_ptr dic, std::function<void(const std::string&, paio::ObjectContainer&)> f) {
    for (auto& [key, value] : dic->map){
        f(key, value);
    }
  }
};
