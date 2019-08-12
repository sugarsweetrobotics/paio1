#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <sstream>
#include <paio/proc/process.h>
#include "paio/proc/process_dictionary.h"

struct MSG {
  std::string body;
};

const std::string string_name = paio::demangle_name(typeid(std::string).name());
       
SCENARIO("Process Dictionary Basic Test", "[process]")
{
  GIVEN("Function Process") {
    auto dic = paio::process_dictionary();
    auto proc = paio::process("my_func1", +[](const std::string& c) {
        return std::string("Name is " + c);
      });

    THEN("Process has Name") {
      paio::put(dic, "my_func1", proc);
      auto p2 = paio::get(dic, "my_func1");
      REQUIRE(p2.name == "my_func1");
    }

    THEN("Process has Name") {
      paio::put(dic, "my_func1", proc);
      auto p2 = paio::get(dic, "my_func1");
      auto c = paio::call<std::string, const std::string&>(p2, "Hello2");
      REQUIRE(c == "Name is Hello2");
    }
  }
}