#pragma once

#include <utility>
#include <functional>

#include <paio/obj/object_server.h>

namespace paio {

class ObjectBroker : public ObjectBase<ObjectServer> {
public:
    ObjectBroker(ObjectServer&& objectServer) : ObjectBase<ObjectServer>(objectServer) {}
};


ObjectBroker object_broker() {
    return ObjectBroker(paio::object_server());
}


}