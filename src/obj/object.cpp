#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/obj/object_server.h>


static const std::function<paio::ObjectContainer(paio::ObjectContainer&&)> ocForwarder = [](paio::ObjectContainer&& oc) { return std::move(oc); };

//static paio::ObjectBase nullobj;

paio::ObjectDictionary_ptr paio::object_dictionary() {
  return std::make_shared<paio::ObjectDictionary>();
}

void paio::put(ObjectDictionary_ptr dic, const std::string &key, paio::ObjectContainer &&oc)
{
  auto cc = paio::getWithDefault(dic->onPutObjectContainers, key, ocForwarder);
  dic->map[key] = cc(std::move(oc));
}