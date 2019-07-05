#pragma once

#include <tuple>

namespace paio
{

template <typename T>
struct is_tuple : std::false_type
{
};

template <typename... Types>
struct is_tuple<std::tuple<Types...>> : std::true_type
{
};

template <typename T>
constexpr bool is_tuple_v = is_tuple<T>::value;


template <typename U, typename... R, typename... S>
std::vector<std::string> tuple_type_names(std::function<std::tuple<R...>(U, S...)> fp = nullptr)
{
    return {demangle_name(typeid(R).name())...};
}


} // namespace paio