#pragma once



#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/obj/json.h>
#include <paio/http/server.h>

namespace paio {

using Jsonizer = std::optional<std::function<paio::json::Container(const paio::ObjectContainer &)>>;

template <typename T>
Jsonizer jsonizer(paio::json::Container (*f)(const T &))
{
    if (!f) return std::nullopt;

    return [f](const paio::ObjectContainer &oc) {
        return f(paio::get<T>(oc));
    };
}

const Jsonizer nullJsonizer = std::nullopt;

using Containizer = std::optional<std::function<paio::ObjectContainer(const paio::json::Container &)>>;
template <typename T>
Containizer containizer(T (*f)(const paio::json::Container &))
{
    if (!f) return std::nullopt;
    return [f](const paio::json::Container &jc) {
        return ObjectContainer(f(jc));
    };
}

const Containizer nullContainizer = std::nullopt;

class JsonIO {
public:
    Jsonizer jsonizer;
    Containizer containizer;

    JsonIO(const Jsonizer& jsonizer, const Containizer& containizer) : jsonizer(jsonizer), containizer(containizer) {}

    paio::json::Container operator()(const paio::ObjectContainer& oc) { return jsonizer.value()(oc); }
    paio::ObjectContainer operator()(const paio::json::Container& jc) { return containizer.value()(jc); }
};

};