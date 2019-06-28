#pragma once

#include <iostream>

#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/obj/json.h>
#include <paio/http/server.h>

#include <paio/obj/object_jsonizer.h>
//#include <paio/obj/object_containizer.h>

namespace paio
{

template <typename T, typename U>
inline void putIf(std::shared_ptr<std::map<T, U>> &map, const T &key, U &value)
{
    if (value)
        (*map)[key] = value;
}

class JsonizerContainer
{
private:
    std::shared_ptr<std::map<std::string, Jsonizer>> map;

public:
    JsonizerContainer() : map(std::make_shared<std::map<std::string, Jsonizer>>()) {}
    JsonizerContainer(JsonizerContainer &&jc) : map(std::move(jc.map)) {}
    JsonizerContainer(const JsonizerContainer &jc) : map(jc.map) {}

public:
    void put(const std::string &key, const Jsonizer &jsonizer)
    {
        if (jsonizer)
            (*map)[key] = jsonizer;
    }

    Jsonizer get(const std::string &key)
    {
        if (map->count(key) != 0)
            return (*map)[key];
        return std::nullopt;
    }

    JsonizerContainer &operator=(JsonizerContainer &&jc)
    {
        this->map = std::move(jc.map);
        return *this;
    }
};

using JsonizerContainer_ptr = std::shared_ptr<JsonizerContainer>;

class ContainizerContainer
{
private:
    std::shared_ptr<std::map<std::string, Containizer>> map;

public:
    ContainizerContainer() : map(std::make_shared<std::map<std::string, Containizer>>()) {}
    ContainizerContainer(ContainizerContainer &&jc) : map(std::move(jc.map)) {}
    ContainizerContainer(const ContainizerContainer &jc) : map(jc.map) {}

public:
    void put(const std::string &key, const Containizer &containizer)
    {
        if (containizer)
            (*map)[key] = containizer;
    }

    Containizer get(const std::string &key)
    {
        if (map->count(key) != 0)
            return (*map)[key];
        return std::nullopt;
    }

    ContainizerContainer &operator=(ContainizerContainer &&jc)
    {
        this->map = std::move(jc.map);
        return *this;
    }
};

using ContainizerContainer_ptr = std::shared_ptr<ContainizerContainer>;

using OnJsonRead = std::function<paio::json::Container(paio::json::Container &&)>;

using OnJsonWrite = std::function<paio::json::Container(paio::json::Container &&)>;

using OnJsonReadMap = std::map<std::string, paio::OnJsonRead>;

using OnJsonReadMap_ptr = std::shared_ptr<OnJsonReadMap>;

using OnJsonWriteMap = std::map<std::string, paio::OnJsonWrite>;

using OnJsonWriteMap_ptr = std::shared_ptr<OnJsonWriteMap>;

class ObjectServer
{
private:
    JsonizerContainer_ptr jsonizers;
    ContainizerContainer_ptr containizers;
    paio::ObjectDictionary_ptr od;
    paio::http::Server_ptr srv;
    OnJsonReadMap_ptr onJsonReads;
    OnJsonWriteMap_ptr onJsonWrites;

    using Action = std::function<paio::ObjectServer(paio::ObjectServer &&)>;

public:
    ObjectServer() : jsonizers(std::make_shared<JsonizerContainer>()), containizers(std::make_shared<ContainizerContainer>()), od(paio::object_dictionary()), srv(nullptr),
                     onJsonReads(std::make_shared<OnJsonReadMap>()), onJsonWrites(std::make_shared<OnJsonWriteMap>()) {}

    ObjectServer(const ObjectServer &os) : jsonizers(os.jsonizers), containizers(os.containizers), od(os.od), srv(os.srv),
                                           onJsonReads(os.onJsonReads), onJsonWrites(os.onJsonWrites) {}

    ObjectServer(ObjectServer &&os) : jsonizers(std::move(os.jsonizers)), containizers(std::move(os.containizers)), od(std::move(os.od)),
                                      srv(std::move(os.srv)), onJsonReads(std::move(os.onJsonReads)), onJsonWrites(std::move(os.onJsonWrites)) {}

    ObjectServer &operator=(ObjectServer &&os)
    {
        this->jsonizers = std::move(os.jsonizers);
        this->containizers = std::move(os.containizers);
        this->od = std::move(os.od);
        this->srv = os.srv;
        this->onJsonReads = std::move(os.onJsonReads);
        this->onJsonWrites = std::move(os.onJsonWrites);
        return *this;
    }

    ObjectServer operator>>=(std::function<paio::http::Server_ptr(paio::http::Server_ptr &&)> f)
    {
        this->srv = f(std::move(srv));
        return std::move(*this);
    }

    ObjectServer &does()
    {
        return *this;
    }

    template <typename... P>
    ObjectServer &does(std::function<paio::http::Server_ptr(paio::http::Server_ptr &&)> f, P... rem)
    {
        srv = std::forward<paio::http::Server_ptr>(f(std::move(srv)));
        return does(rem...);
    }

public:
    friend std::function<paio::ObjectServer(paio::ObjectServer &&)> startUnlock(const std::string &address, const int32_t port);

    template <typename T>
    friend std::function<paio::ObjectServer(paio::ObjectServer &&)> registerObject(const std::string &topic, const T &obj, const Jsonizer &jsonizer, const Containizer &containizer);

    friend ObjectServer::Action registerOnJsonRead(const std::string &topicName, paio::OnJsonRead f);

    friend ObjectServer::Action registerOnJsonWrite(const std::string &topicName, paio::OnJsonWrite f);

    friend ObjectServer::Action registerOnPutObjectContainer(const std::string &key, OnPutObjectContainer onPut);
};

ObjectServer object_server();

template <typename T>
std::function<paio::ObjectServer(paio::ObjectServer &&)> registerObject(const std::string &topic, const T &obj, const Jsonizer &jsonizer, const Containizer &containizer)
{
    return [&topic, obj, jsonizer, containizer](ObjectServer &&os) {
        os.jsonizers->put(typeid(T).name(), jsonizer);
        os.containizers->put(typeid(T).name(), containizer);
        paio::put(os.od, topic, std::move(obj));
        return os;
    };
}

template <typename T>
std::function<paio::ObjectServer(paio::ObjectServer &&)> registerObject(const std::string &topic, const T &obj, paio::json::Container (*f)(const T &) = nullptr, T (*c)(const paio::json::Container &) = nullptr)
{
    return registerObject(topic, obj, jsonizer<T>(f), containizer<T>(c));
}

std::function<paio::ObjectServer(paio::ObjectServer &&)> startUnlock(const std::string &address, const int32_t port);

//template<typename T>
//concept functype = requires(T a, paio::ObjectContainer&& oc)
//{
//   { a(oc) } -> paio::ObjectContainer;
//};

inline ObjectServer::Action registerOnJsonRead(const std::string &topicName, paio::OnJsonRead f)
{
    return [topicName, f](paio::ObjectServer &&os) {
        os.onJsonReads->emplace(topicName, f);
        return os;
    };
}
inline ObjectServer::Action registerOnJsonWrite(const std::string &topicName, paio::OnJsonWrite f)
{
    return [topicName, f](paio::ObjectServer &&os) {
        os.onJsonWrites->emplace(topicName, f);
        return os;
    };
}

inline ObjectServer::Action registerOnPutObjectContainer(const std::string &topicName, OnPutObjectContainer f)
{
    return [topicName, f](paio::ObjectServer &&os) {
        paio::registerOnPutObjectContainer(os.od, topicName, f);
        return os;
    };
}

} // namespace paio