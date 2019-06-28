#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <paio/obj/object_dictionary.h>

using namespace paio;

struct TM { // TestMessage
  std::string h; // header
  std::string b; // body
};

bool operator==(const TM& m1, const TM& m2) {
  return (m1.h == m2.h) && (m1.b == m2.b);
}


SCENARIO( "Object", "[object]" ) {

  GIVEN("Object Dictionary insert without error") {
    auto od = paio::object_dictionary();
    const auto obj = paio::object<TM>({"header", "body"});

    REQUIRE(( obj >>= +[](const TM& tm) { return tm.h == "header"; }));
    REQUIRE(( obj >>= +[](const TM& tm) { return tm.b == "body"; }));
    THEN("Object Dictionary insertion") {
      paio::put(od, "/topic01", obj);
    }

    THEN("Object Dictionary load") {
      paio::put(od, "/topic01", obj);
      auto obj2 = paio::get<TM>(od, "/topic01");
      REQUIRE(!paio::isNull(obj));
      REQUIRE(( obj >>= +[](const TM& tm) { return tm.h == "header"; }));
      REQUIRE(!paio::isNull(obj2));
      REQUIRE(( obj2 >>= +[](const TM& tm) { return tm.h == "header"; }));
      //REQUIRE(obj2 == obj);
    }

  }
}


