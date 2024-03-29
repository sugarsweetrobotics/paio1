#pragma once

#include <memory>
#include "paio/proc/process_function.h"

namespace paio
{

class Process {

public:
    static Process Null;
public:
    std::string name;
    std::shared_ptr<IProcessFunction> fp;
public:
    Process(): name("null"), fp(nullptr) {}
    Process(const Process& p) : name(p.name), fp(p.fp) {}
    Process(Process&& p) : name(std::move(p.name)), fp(std::move(p.fp)) {}
    explicit Process(const std::string& n, IProcessFunction* func): name(n), fp(func) {}
    virtual ~Process() {}

public:
    Process& operator=(const Process& p) {
        this->name = p.name;
        this->fp = p.fp;
        return *this;
    }
    friend bool isNull(const paio::Process& proc);
};

template<typename T, typename... R>
Process process(const std::string& name, T(*f)(R...)) {
  std::cout << "Constructing Process...." << std::endl;
    std::function<T(R...)> fp = f;
    return Process(name, paio::processFunction(fp));
}

template<typename T, typename...R>
inline std::optional<T> call(const Process& p, R... args) {
  std::cout << "Calling Proces.." << std::endl;
  auto fp = std::dynamic_pointer_cast<ProcessFunction<T, R...>>(p.fp).get();
  std::cout << "fp olay" << std::endl;
  auto ret =  paio::call<T, R...>(fp, args...);
  std::cout << "ret" << std::endl;
  return ret;
}


inline bool isNull(const paio::Process& proc) {
    return !(proc.fp);
}

} // namespace paio

