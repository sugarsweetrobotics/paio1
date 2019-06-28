#pragma once

#include "paio/obj/object.h"

namespace paio
{

using ObjectList = std::list<paio::ObjectContainer>;

using ProcessFunction = std::optional<std::function<ObjectList(const ObjectList &)>>;

tempale<typename T>
    ProcessFunction func(F f)
{
}

class Process
{
private:
    ProcessFunction func;

public:
    Process() {}

    ~Process();

public:
    friend bool isNull(const Process &process);
};

bool isNull(const Process &process)
{
    return process.func;
}

} // namespace paio