#pragma once

#include <iostream>
#include <map>
#include <string>
#include <paio/obj/object.h>
#include <paio/obj/object_container.h>
#include "paio/proc/process.h"


namespace paio
{

using OnPutProcess = std::function<paio::Process(paio::Process&&)>;
using OnPutProcessMap = std::map<std::string, OnPutProcess>;
using OnPutProcessMap_ptr = std::shared_ptr<OnPutProcessMap>;


class ProcessDictionary;
using ProcessDictionary_ptr = std::shared_ptr<ProcessDictionary>;

struct ProcessDictionary
{
private:
  OnPutProcessMap_ptr onPutProcesses;
public:
  std::map<std::string, Process> map;

public:
  ProcessDictionary(): onPutProcesses(std::make_shared<OnPutProcessMap>()), map() {}

  //friend Process registerOnPutProcess(Process_ptr dic, const std::string& key, OnPutProcess onPut);

  friend void put(ProcessDictionary_ptr dic, const std::string &key, paio::Process &&oc);
  friend void put(ProcessDictionary_ptr dic, const std::string &key, const paio::Process &oc);
  friend Process& get(ProcessDictionary_ptr dic, const std::string& key);
};

ProcessDictionary_ptr process_dictionary() {
    return std::make_shared<ProcessDictionary>();
}

void put(ProcessDictionary_ptr dic, const std::string &key, const paio::Process &proc)
{
    dic->map[key] = proc;
}

void put(ProcessDictionary_ptr dic, const std::string &key, paio::Process &&proc)
{
    dic->map[key] = std::move(proc);
}

Process& get(ProcessDictionary_ptr dic, const std::string &key)
{
  if (dic->map.count(key) == 0)
  {
    return paio::Process::Null;
  }
  return dic->map[key];
}

Process paio::Process::Null;
/*


>>>>>>> 555d268b0235859fb3bb8bba6d67ca91471fadab
template<typename T>
T getContainer(ObjectDictionary_ptr dic, const std::string& key, std::function<T(const paio::ObjectContainer&)> f) {
  if (dic->map.count(key) == 0)
  {
    return f(paio::ObjectContainer());
  }
  return f(dic->map[key]);
}
<<<<<<< HEAD

template <typename T>
void put(ObjectDictionary_ptr dic, const std::string &key, paio::Object<T> &&doc)
{
  put(dic, key, ObjectContainer(doc._get()));
}

=======
*/

/*
>>>>>>> 555d268b0235859fb3bb8bba6d67ca91471fadab
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
<<<<<<< HEAD

=======
*/

}; // namespace paio
