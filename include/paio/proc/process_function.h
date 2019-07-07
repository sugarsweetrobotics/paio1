#pragma once

#include <list>

#include <utility>
#include <type_traits>
#include <tuple>
#include "paio/obj/mangling.h"
#include "paio/obj/object.h"
#include "paio/obj/object_dictionary.h"

#include "paio/proc/utility.h"

namespace paio
{

class IProcessFunction
{
public:
    virtual ~IProcessFunction() {}

public:
    virtual std::vector<std::string> argumentTypeNames() = 0;

    virtual std::vector<std::string> returnTypeNames() = 0;

    virtual IProcessFunction *bindAny(std::shared_ptr<void> &arg) = 0;

    virtual std::optional<std::shared_ptr<void>> callAny(std::shared_ptr<void> arg) = 0;
};

template <typename T, typename U, typename... R>
class ProcessFunction;

template <typename T>
inline auto processFunction(std::function<T()> fp)
{
    return nullptr;
}

template <typename T, typename U, typename... R>
inline ProcessFunction<T, U, R...>* processFunction(std::function<T(U, R...)> fp)
{
    return fp ? new ProcessFunction<T, U, R...>(fp) : nullptr;
}

template <typename T, typename U, typename S, typename... R>
std::optional<T> call(ProcessFunction<T, U, S, R...> *pf, U arg1)
{
    return std::nullopt;
}

template <typename T, typename U, typename S, typename... R>
std::optional<T> call(ProcessFunction<T, U, S, R...> *pf, U arg1, S arg2, R... args)
{
    return call<T, S, R...>(pf->bind(arg1), arg2, args...);
}

template <typename T, typename U>
std::optional<T> call(ProcessFunction<T, U> *pf, U &arg)
{
    return pf->f(arg);
}


template <typename T, typename U, typename... R>
class ProcessFunction : public IProcessFunction
{
private:

public:
    std::function<T(U, R...)> f;
    using ReturnType = T;
    using ArgumentType = U;


    ProcessFunction(std::function<T(U, R...)> fp) : f(fp) {}

    template <typename P> ProcessFunction(P fp) : f(fp) {}

    virtual ~ProcessFunction() {}

private:

    std::function<T(R...)> bindFunction(U arg)
    {
        if constexpr (sizeof...(R) == 0)
        {
            return nullptr;
        } else {
            return std::function<T(R...)>([*this, arg](auto... args) {
                return this->f(arg, args...);
            });
        }
    }

public:
    virtual std::vector<std::string> argumentTypeNames()
    {
        return {demangle_name(typeid(U).name()), demangle_name(typeid(R).name())...};
    }

    virtual std::vector<std::string> returnTypeNames()
    {
        if constexpr (is_tuple<T>::value)
        {
            return tuple_type_names(f);
        }
        return {demangle_name(typeid(T).name())};
    }

    virtual IProcessFunction* bindAny(std::shared_ptr<void> &arg)
    {
        return static_cast<IProcessFunction*>(bind(*(std::static_pointer_cast<std::decay_t<U>>(arg))));
    }

    auto bind(U arg)
    {
        return processFunction<T, R...>(this->bindFunction(arg));
    }

    std::optional<T> call(U arg)
    {
        return paio::call<T, U, R...>(this, arg);
    }

    std::optional<std::shared_ptr<void>> callAny(std::shared_ptr<void> arg)
    {
        auto t = paio::call<T, U, R...>(this, *(std::static_pointer_cast<std::decay_t<U>>(arg)));
        if (!t)
        {
            return std::nullopt;
        }
        auto retval = std::shared_ptr<std::decay_t<T>>(new T(t.value()));
        return std::static_pointer_cast<void>(retval);
    }
};

} // namespace paio