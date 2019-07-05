#pragma once

#include <memory>
#include "paio/proc/process_function.h"

namespace paio
{

class Process {
public:
    const std::string name;
    const std::shared_ptr<IProcessFunction> fp;
public:
    
    Process(const Process& p) : name(p.name), fp(p.fp) {}
    Process(Process&& p) : name(std::move(p.name)), fp(std::move(p.fp)) {}
    explicit Process(const std::string& n, IProcessFunction* func): name(n), fp(func) {}
    virtual ~Process() {}

public:
    
};

template<typename T, typename... R>
Process process(const std::string& name, T(*f)(R...)) {
    std::function<T(R...)> fp = f;
    return Process(name, paio::processFunction(fp));
}


} // namespace paio