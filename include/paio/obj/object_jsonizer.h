#pragma once



#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/obj/json.h>
#include <paio/http/server.h>

namespace paio {

using Jsonizer = std::function<paio::json::Container(const paio::ObjectContainer &)>;

template <typename T>
Jsonizer jsonizer(paio::json::Container (*f)(const T &))
{
    return [f](const paio::ObjectContainer &oc) {
        return f(paio::get<T>(oc));
    };
}

};