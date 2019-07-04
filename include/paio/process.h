#pragma once

#include <list>

#include <utility>
#include <type_traits>
#include <tuple>
#include "paio/obj/mangling.h"
#include "paio/obj/object.h"
#include "paio/obj/object_dictionary.h"

namespace paio
{


    //! Tests if T is a specialization of Template
    template <typename T, template <typename...> class Template>
    struct is_specialization_of : std::false_type {};
    template <template <typename...> class Template, typename... Args>
    struct is_specialization_of<Template<Args...>, Template> : std::true_type {};


    template < typename T >
struct is_tuple : std::false_type{};

template < typename ...Types >
struct is_tuple<std::tuple<Types...>> : std::true_type {};

template < typename T >
constexpr bool is_tuple_v = is_tuple<T>::value;

    class IProcessFunction {
    public:
        ~IProcessFunction() {}

    public:
        virtual IProcessFunction* bindAny(std::shared_ptr<void>& arg) = 0;
        
        virtual std::optional<std::shared_ptr<void>> callAny(std::shared_ptr<void> arg) = 0;
    };

    template<typename T, typename U, typename...R>
    class ProcessFunction;

    template<typename T>
    inline auto processFunction(std::function<T()> fp) {
        return nullptr;
    }

    template<typename T, typename U, typename...R>
    inline ProcessFunction<T, U, R...>* processFunction(std::function<T(U, R...)> fp) {
        if (fp) {
            return new ProcessFunction<T, U, R...>(fp);
        }
        return nullptr;
    }

    template<typename T, typename U, typename S, typename...R>
    std::optional<T> call(ProcessFunction<T, U, S, R...>* pf, U arg) {
        return std::nullopt;
    }

    template<typename T, typename U>
    std::optional<T> call(ProcessFunction<T, U>* pf, U& arg) {
        return pf->f(arg);
    }

    template<typename U, typename... R, typename... S>
    std::vector<std::string> tuple_type_names(std::function<std::tuple<R...>(U, S...)> fp = nullptr) {
        return {demangle_name(typeid(R).name())...};
    }

    template<typename T, typename U, typename...R>
    class ProcessFunction : public IProcessFunction {
    public:
        using ReturnType = T;
        using ArgumentType = U;

        std::function<T(U, R...)> f;

        //ProcessFunction(std::function<T(R...)> fp) : f(fp) {}

        ProcessFunction(std::function<T(U, R...)> fp) : f(fp) {}

        template<typename P>
        ProcessFunction(P fp) : f(fp) {}

    public:
        std::vector<std::string> argumentTypeNames() {
            return {demangle_name(typeid(U).name()), demangle_name(typeid(R).name())...};
        }
   
   
        std::vector<std::string>
        returnTypeNames() {
            if constexpr (is_tuple<T>::value) {
                return  tuple_type_names(f);
            }
            return {demangle_name(typeid(T).name())};
        }


        std::function<T(R...)> bindFunction(U arg) {
            if (sizeof...(R) == 0) {
                return nullptr;
            }
            return std::function<T(R...)>([*this, arg](auto... args) {
                return this->f(arg, args...);
            });
        }

        virtual IProcessFunction* bindAny(std::shared_ptr<void>& arg) {
            return static_cast<IProcessFunction*>(bind(*(std::static_pointer_cast<std::decay_t<U>>(arg))));
        }
        
        auto bind(U arg) {
            return processFunction<T, R...>(this->bindFunction(arg));
        }

        std::optional<T> call(U arg) {
            return paio::call<T, U, R...>(this, arg);
        }

        std::optional<std::shared_ptr<void>> callAny(std::shared_ptr<void> arg) {
            auto pt = std::static_pointer_cast<std::decay_t<U>>(arg);
            auto t = paio::call<T, U, R...>(this, *pt);
            if (!t) {
                return std::nullopt;
            }
            auto retval = std::shared_ptr<std::decay_t<T>>(new T(t.value()));
            return std::static_pointer_cast<void>(retval);
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