#pragma once
#include <iostream>
#include <map>
#include <string>
#include <paio/obj/object.h>

namespace paio
{

template <typename T, typename U>
inline U getWithDefault(const std::shared_ptr<std::map<T, U>> &map, const T &key, const U &defaultValue)
{
    if (map->count(key) != 0)
        return map->at(key);
    return defaultValue;
}

class ObjectContainer : public ObjectBase<std::shared_ptr<void>>
{
public:
  std::string typeName;
  
  ObjectContainer() : ObjectBase<std::shared_ptr<void>>(nullptr), typeName("void"){}

  ObjectContainer(const std::shared_ptr<void>&& v, const std::string&& typeName): ObjectBase<std::shared_ptr<void>>(std::move(v)), typeName(std::move(typeName))  {}

  ObjectContainer(ObjectContainer&& oc): ObjectContainer(std::move(oc._privateData), std::move(oc.typeName)) {}

  ObjectContainer& operator=(ObjectContainer&& oc) {
    this->_privateData = std::move(oc._privateData);
    this->typeName = std::move(oc.typeName);
    return *this;
  }

  template <typename T>
  ObjectContainer(const std::shared_ptr<T> data) : ObjectBase<std::shared_ptr<void>>(std::static_pointer_cast<void>(data)), typeName(typeid(T).name()) {}

  template <typename T>
  ObjectContainer(const T &data) : ObjectContainer(std::make_shared<T>(data)) {}

public:
  template <typename T, typename W>
  std::optional<W> bind(std::function<W(const T &)> f)
  {
    if (!this->_privateData)
    {
      return std::nullopt;
    }
    return f(*(std::static_pointer_cast<const T>(this->get())));
  }
};

inline bool isNull(const ObjectContainer& oc) {
  return !(oc.get());
}

template <typename T>
T get(const ObjectContainer &oc)
{
  return *(std::static_pointer_cast<const T>(oc.get()));
}

using OnPutObjectContainer = std::function<paio::ObjectContainer(paio::ObjectContainer&&)>;
using OnPutObjectContainerMap = std::map<std::string, OnPutObjectContainer>;
using OnPutObjectContainerMap_ptr = std::shared_ptr<OnPutObjectContainerMap>;

class ObjectDictionary;
using ObjectDictionary_ptr = std::shared_ptr<ObjectDictionary>;

struct ObjectDictionary
{
private:
  OnPutObjectContainerMap_ptr onPutObjectContainers;
public:
  std::map<std::string, ObjectContainer> map;

public:
  ObjectDictionary(): onPutObjectContainers(std::make_shared<OnPutObjectContainerMap>()), map() {}

  friend ObjectDictionary_ptr registerOnPutObjectContainer(ObjectDictionary_ptr dic, const std::string& key, OnPutObjectContainer onPut);

  friend void put(std::shared_ptr<ObjectDictionary> dic, const std::string &key, paio::ObjectContainer &&oc);

};

ObjectDictionary_ptr object_dictionary();

template <typename T>
paio::Object<T> get(ObjectDictionary_ptr dic, const std::string &key)
{
  if (dic->map.count(key) == 0)
  {
    return paio::Object<T>();
  }
  return paio::Object<T>(std::static_pointer_cast<T>(dic->map[key].get()));
}

template<typename T>
T getContainer(ObjectDictionary_ptr dic, const std::string& key, std::function<T(const paio::ObjectContainer&)> f) {
  if (dic->map.count(key) == 0)
  {
    return f(paio::ObjectContainer());
  }
  return f(dic->map[key]);
}

template <typename T>
void put(ObjectDictionary_ptr dic, const std::string &key, paio::Object<T> &&doc)
{
  put(dic, key, ObjectContainer(doc._get()));
}

template <typename T>
void put(ObjectDictionary_ptr dic, const std::string &key, const paio::Object<T> &doc)
{
  put(dic, key, ObjectContainer(*doc.get()));
}

void put(std::shared_ptr<ObjectDictionary> dic, const std::string &key, paio::ObjectContainer &&oc);


inline void forEach(ObjectDictionary_ptr dic, void (*f)(const std::string &, paio::ObjectContainer &))
{
  for (auto &[key, value] : dic->map)
  {
    f(key, value);
  }
}

inline void forEach(ObjectDictionary_ptr dic, std::function<void(const std::string &, paio::ObjectContainer &)> f)
{
  for (auto &[key, value] : dic->map)
  {
    f(key, value);
  }
}

inline ObjectDictionary_ptr registerOnPutObjectContainer(ObjectDictionary_ptr dic, const std::string& key, OnPutObjectContainer onPut) {
  (*(dic->onPutObjectContainers))[key] = onPut;
  return dic;
}

}; // namespace paio
