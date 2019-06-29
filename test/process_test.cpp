#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>


#include <paio/process.h>

struct MSG {
  std::string body;
};

std::string arg(const paio::ObjectList& ol) {
  return paio::get<MSG>(ol[0]);
}

SCENARIO("Process Basic Test", "[process]")
{
  GIVEN("Null Process") {
    auto p = paio::process();

    REQUIRE(paio::isNull(p));
  }

  GIVEN("Simple Function") {
    auto p = paio::process([](std::string& msg) {
      return msg + " is hoge";
    });


  }
}