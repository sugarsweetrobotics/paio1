#pragma once

#include <iostream>

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

using Containizer = std::function<paio::ObjectContainer(const paio::json::Container &)>;
template <typename T>
Containizer containizer(T(*f)(const paio::json::Container &))
{
    return [f](const paio::json::Container &jc) {
        return ObjectContainer(f(jc));
    };
}

class ObjectServer
{
private:
    std::shared_ptr<std::map<std::string, Jsonizer>> js;
    std::shared_ptr<std::map<std::string, Containizer>> cs;
    paio::ObjectDictionary_ptr od;

public:
    paio::http::Server_ptr srv;
    ObjectServer() : od(paio::object_dictionary()), js(std::make_shared<std::map<std::string, Jsonizer>>()), cs(std::make_shared<std::map<std::string, Containizer>>()) {}

    ObjectServer(ObjectServer&& os) : js(std::move(os.js)), cs(std::move(os.cs)), od(std::move(os.od)), srv(std::move(os.srv)){
    }

    ObjectServer& operator=(ObjectServer&& os) {
        this->js = std::move(os.js);
        this->cs = std::move(os.cs);
        this->od = (os.od);
        this->srv = os.srv;
        return *this;
    }
public:
    friend ObjectServer startUnlock(ObjectServer &&server, const std::string &address, const int32_t port);

    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj);

    template <typename T>
    friend ObjectServer put(ObjectServer &&os, const std::string &topic, const Object<T> &obj);

    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, Jsonizer jsonizer);

    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, Jsonizer jsonizer, Containizer containizer);

    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, paio::json::Container (*f)(const T &));

    template <typename T>
    friend ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, paio::json::Container (*f)(const T &), T (*c)(const paio::json::Container &));


    friend ObjectServer startUnlock(ObjectServer &&server, const std::string &address, const int32_t port);
};

ObjectServer object_server();

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj)
{
    paio::put(os.od, topic, obj);
    return std::move(os);
}

template <typename T>
ObjectServer put(ObjectServer &&os, const std::string &topic, const Object<T> &obj)
{
    paio::put(os.od, topic, obj);
    return std::move(os);
}

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, Jsonizer jsonizer)
{
    paio::put(os.od, topic, obj);
    (*os.js)[typeid(T).name()] = jsonizer;
    return std::move(os);
}

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, Jsonizer jsonizer, Containizer containizer)
{
    paio::put(os.od, topic, obj);
    (*os.js)[typeid(T).name()] = jsonizer;
    (*os.cs)[typeid(T).name()] = containizer;
    return std::move(os);
}

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, paio::json::Container (*f)(const T &))
{
    paio::put(os.od, topic, obj);
    (*os.js)[typeid(T).name()] = paio::jsonizer(f);
    return std::move(os);
}

template <typename T>
ObjectServer registerObject(ObjectServer &&os, const std::string &topic, const Object<T> &obj, paio::json::Container(*f)(const T &), T(*c)(const paio::json::Container &))
{
    paio::put(os.od, topic, obj);
    (*os.js)[typeid(T).name()] = paio::jsonizer(f);
    (*os.cs)[typeid(T).name()] = paio::containizer(c);
    return std::move(os);
}

ObjectServer startUnlock(ObjectServer &&server, const std::string &address, const int32_t port);

} // namespace paio