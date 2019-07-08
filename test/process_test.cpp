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
    auto proc = paio::process("my_func1", +[](const std::string& c) {
        return std::string("Name is " + c);
      });

    THEN("Process has Name") {
      REQUIRE(proc.name == "my_func1");
    }

    THEN("Can call") {
      auto a = paio::call<std::string, const std::string&>(proc, "Hello").value();
      REQUIRE(a == "Name is Hello");
    }
  }

  GIVEN("Tuple Output Function Process") {
    auto proc = paio::process("my_func", +[](const std::string& c) {
        return std::make_tuple(std::string("Name is " + c), std::string("Value is " + c));
      });

    THEN("Can call") {
      auto [a,b] = paio::call<std::tuple<std::string, std::string>, const std::string&>(proc, "Hello2").value();

      REQUIRE(a == "Name is Hello2");
      REQUIRE(b == "Value is Hello2");
    }
  }
}