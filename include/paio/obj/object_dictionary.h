#pragma once

#include <map>
//#include <paio/paio.h>
#include <string>
#include <paio/obj/object.h>

namespace paio {

  
  struct ObjectDictionary {
    std::map<std::string, paio::ObjectBase> map;
  };


  using ObjectDictionary_ptr = paio::ptr<ObjectDictionary>;

  ObjectDictionary_ptr object_dictionary();

  inline paio::ObjectBase get(ObjectDictionary_ptr dic, const std::string& key) {
    if (dic->map.count(key) == 0) {
      return paio::ObjectBase();
    }
    return dic->map[key];
  }

  inline void put(ObjectDictionary_ptr dic, const std::string& key, paio::ObjectBase doc) {
    dic->map[key] = doc;
  }
};
