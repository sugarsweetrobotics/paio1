#pragma once

#include <list>
#include "paio/obj/object.h"
#include "paio/obj/object_dictionary.h"

namespace paio
{

    template<typename T, typename U, typename...R>
    class ProcessFunction;

    template<typename T, typename U, typename...R>
    ProcessFunction<T, U, R...>* processFunction(std::function<T(U, R...)> fp) {
        return new ProcessFunction<T, U, R...>(fp);
    }

    template<typename T, typename U, typename...R>
    class ProcessFunction {
    public:
        std::function<T(U, R...)> f;

        //ProcessFunction(std::function<T(R...)> fp) : f(fp) {}

        ProcessFunction(std::function<T(U, R...)> fp) : f(fp) {}

        template<typename P>
        ProcessFunction(P fp) : f(fp) {}

    public:
        std::vector<std::string> typeName() {
            return std::vector<std::string>({typeid(U).name(), typeid(R).name()...});
        }

        auto bindType(U arg) {
            return processFunction(this->bind(arg));
        }

        std::function<T(R...)> bind(U arg) {
            return std::function<T(R...)>([*this, arg](auto... args) {
                return this->f(arg, args...);
            });
        }
    };
#if 0
template<typename T, typename U, typename W>
class ProcessFunction {
public:
    template<typanem G>
    ProcessFunction<T, W, G> child;
    //U* param;
    //ProcessFunction<T, R...>  child;
public:
    ProcessFunction(std::function<T()> func)  {

    }

    template<typename U>
    ProcessFunction(std::function<T(U, R...)> func)  {

    }
};
#endif

/*

class Process : public paio::ObjectBase<std::shared_ptr<void>>
{
public:
    Process() : ObjectBase<std::shared_ptr<void>>(nullptr) {}
    
    virtual ~Process() {}

    Process(ProcessFunction_ptr fp) : ObjectBase<std::shared_ptr<void>>(std::static_pointer_cast<void>(fp)) {}

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
    return !(process.get());
}
*/

} // namespace paio