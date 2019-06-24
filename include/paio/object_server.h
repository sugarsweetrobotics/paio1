#pragma once

#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/obj/json.h>
#include <paio/http/server.h>

namespace paio
{
using Jsonizer = std::function<paio::json::Container(const paio::ObjectContainer &)>;

template <typename T>
Jsonizer jsonizer(paio::json::Container (*f)(const T &))
{
    return [f](const paio::ObjectContainer &oc) {
        return f(paio::get<T>(oc));
    };
}

using Containizer = std::function<paio::ObjectContainer(paio::json::Container&)>;
template <typename T>
Containizer containizer(T(*f)(const paio::json::Container&))
{    return [f](const paio::json::Container &jc) {
        return paio::object<T>(f(jc));
    };
}


class ObjectServer
{
private:
    std::map<std::string, Jsonizer> js;
    std::map<std::string, Containizer> cs;
    paio::ObjectDictionary_ptr od;

public:
    paio::http::Server_ptr srv;
    ObjectServer() : od(paio::object_dictionary()) {}

public:
    friend ObjectServer startUnlock(ObjectServer &&server, const std::string &address, const int32_t port);

    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj);

    template <typename T>
    friend ObjectServer put(ObjectServer &&os, const std::string &topic, const Object<T> &obj);

    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, Jsonizer jsonizer);

    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, paio::json::Container (*f)(const T &));
};

ObjectServer object_server();

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj)
{
    paio::put(os.od, topic, obj);
    return os;
}

template <typename T>
ObjectServer put(ObjectServer &&os, const std::string &topic, const Object<T> &obj)
{
    paio::put(os.od, topic, obj);
    return os;
}

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, Jsonizer jsonizer)
{
    paio::put(os.od, topic, obj);
    os.js[typeid(T).name()] = jsonizer;
    return os;
}

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, paio::json::Container (*f)(const T &))
{
    paio::put(os.od, topic, obj);
    os.js[typeid(T).name()] = paio::jsonizer(f);
    return os;
}

inline ObjectServer startUnlock(ObjectServer &&server, const std::string &address, const int32_t port)
{
    server.srv = paio::http::server(address, port);
    std::function<void(const std::string &, paio::ObjectContainer &)> f = [&server](const std::string &topic,
                                                                                    paio::ObjectContainer &container) {
        server.srv = http::serve(topic, "GET", [&](http::Request &&r) {
            if (server.js.count(container.typeName) != 0)
            {
                return http::Response(200, json::stringify(server.js[container.typeName](container)));
            }
            return http::Response(200, std::move(container.typeName));
        }, std::move(server.srv));
        server.srv = http::serve(topic, "PUT", [&](http::Request &&r) {
            
            return http::Response(200, std::move(container.typeName));
        }, std::move(server.srv));
    };
    paio::forEach(server.od, f);
    server.srv = http::listen(1.0, std::move(server.srv));
    return server;
}

} // namespace paio