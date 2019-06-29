#pragma once

#include <list>
#include "paio/obj/object.h"
#include "paio/obj/object_dictionary.h"

namespace paio
{

using ObjectList = std::list<paio::ObjectContainer>;

using ProcessFunction = std::optional<std::function<ObjectList(const ObjectList &)>>;

template<typename T>
ProcessFunction processFunction(T f) {
    return [f] (const ObjectList& ol) {
        return retn(std::apply(f, arg(ol)));
    }
}

class Process
{
private:
    ProcessFunction func;

public:
    Process() : func(std::nullopt) {}

    template<typename T>
    Process(T f) : func(processFunction(f)) {}

    ~Process() {}

public:
    friend bool isNull(const Process &process);
};

Process process() {
    return Process();
}

template<typename T>
Process process(T f) {
    return Process(f);
}

bool isNull(const Process &process)
{
    return !static_cast<bool>(process.func);
}

} // namespace paio