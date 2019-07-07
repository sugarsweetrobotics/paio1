#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <sstream>
#include <paio/proc/process.h>

struct MSG {
  std::string body;
};

const std::string string_name = paio::demangle_name(typeid(std::string).name());
       
SCENARIO("Process Basic Test", "[process]")
{
  GIVEN("Function Process") {
    auto proc = paio::process("my_func", +[](const std::string& c) {
        return std::make_tuple("Name is " + c, "Name was " + c);
      });

    THEN("Can call") {
      auto [a, c] = paio::call<std::tuple<std::string, std::string>, const std::string>(proc, "Hello").value();
    }
  }
}