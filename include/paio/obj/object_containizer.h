#pragma once

#pragma once

#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/obj/json.h>
#include <paio/http/server.h>

namespace paio
{

using Containizer = std::function<paio::ObjectContainer(const paio::json::Container &)>;
template <typename T>
Containizer containizer(T (*f)(const paio::json::Container &))
{
    return [f](const paio::json::Container &jc) {
        return ObjectContainer(f(jc));
    };
}

}; // namespace paio