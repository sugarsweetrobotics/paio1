#pragma once

#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/http/server.h>

namespace paio
{

class ObjectServer
{
private:
    paio::ObjectDictionary_ptr od;
    
public:
    paio::http::Server_ptr srv;
    ObjectServer() : od(paio::object_dictionary()) {}

public:
    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj);
};

ObjectServer object_server();

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj)
{
    paio::put(os.od, topic, obj);
    return os;
}

inline ObjectServer startUnlock(ObjectServer&& server, const std::string& address, const int32_t port) {
    server.srv = paio::http::server(address, port);
    return server;
}


} // namespace paio