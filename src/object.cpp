#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>


static paio::ObjectBase nullobj;

paio::ObjectDictionary_ptr paio::object_dictionary() {
  return std::make_shared<paio::ObjectDictionary>();
}
