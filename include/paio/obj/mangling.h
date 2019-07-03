#pragma once

#include <string>
#include <typeinfo>
#include <cxxabi.h>

namespace paio {

inline std::string demangle_name(const std::string& name) {
    int status;
    return abi::__cxa_demangle(name.c_str(), 0, 0, &status);
}

}