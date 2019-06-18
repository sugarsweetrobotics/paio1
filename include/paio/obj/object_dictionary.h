#pragma once

#include <map>
//#include <paio/paio.h>
#include <string>
#include <paio/obj/object.h>

namespace paio {
  
  struct ObjectDictionary {
  public:
    std::map<std::string, std::shared_ptr<void>> map;
  };

  using ObjectDictionary_ptr = std::shared_ptr<ObjectDictionary>;

  ObjectDictionary_ptr object_dictionary();

  template<typename T>
  paio::Object<T> get(ObjectDictionary_ptr dic, const std::string& key) {
    if (dic->map.count(key) == 0) {
      return paio::Object<T>();
    }
    return Object<T>((std::static_pointer_cast<T>(dic->map[key])));
  }
  
  template<typename T>
  void put(ObjectDictionary_ptr dic, const std::string& key, paio::Object<T>&& doc) {
    dic->map[key] = std::static_pointer_cast<void>(doc._get());
  }

  template<typename T>
  void put(ObjectDictionary_ptr dic, const std::string& key, paio::Object<T>& doc) {
    dic->map[key] = std::static_pointer_cast<void>(doc._get());
  }

};
