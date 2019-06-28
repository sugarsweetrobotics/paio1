#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <functional>
#include <paio/obj/json.h>

using namespace paio;

SCENARIO("JSON test", "[json]")
{
      GIVEN("JSON parse")
      {
            static const char *s_json = R"(
{
    "string" : "foo",
    "number" : 123,
    "array" : [
        0,
        1,
        2,
        3
    ],
    "object" : {
        "v0" : "bar",
        "v1" : 456,
        "v2" : 0.123
    }
}
)";

            WHEN("JSON Parsed")
            {
                  auto j = json::parse(s_json);

                  THEN("Keys")
                  {
                        auto ks = json::keys(j);
                        REQUIRE(std::find(ks.begin(), ks.end(), "string") != ks.end());
                  }

                  THEN("Can Move") {

                        auto b = std::move(j);

                        REQUIRE(json::string(json::get(b, "string")) == "foo");
                        REQUIRE(json::uint32(json::get(b, "number")) == 123);
                  }

                  THEN("Simple Data")
                  {
                        REQUIRE(json::string(json::get(j, "string")) == "foo");
                        REQUIRE(json::uint32(json::get(j, "number")) == 123);
                  }

                  THEN("Array")
                  {
                        REQUIRE(json::uint32(json::get(j, "array")[0]) == 0);
                  }

                  THEN("Not Array")
                  {
                        REQUIRE(json::is_nil(json::get(j, "string")[0]));
                  }

                  THEN("Object")
                  {
                        REQUIRE(json::string(json::get(json::get(j, "object"), "v0")) == "bar");
                        REQUIRE(json::string(json::get(json::get(j, "object"), "v0")) == "bar");
                        REQUIRE(json::uint32(json::get(json::get(j, "object"), "v1")) == 456);
                        REQUIRE(json::float32(json::get(json::get(j, "object"), "v2")) == 0.123f);
                        REQUIRE(json::float64(json::get(json::get(j, "object"), "v2")) == 0.123);
                  }
            }
      }

      GIVEN("JSON consturct")
      {
            WHEN("int value added")
            {
                  auto doc = json::document(json::int32("int_value", 123));
                  THEN("int value is int value")
                  {
                        REQUIRE(json::equals(json::get(doc, "int_value"), 123));
                  }

                  THEN("stringified")
                  {
                        REQUIRE(json::stringify(doc) == "{\"int_value\":123}");
                  }
            }

            WHEN("object value added")
            {
                  auto doc = json::document(json::object("obj_value", json::int32("int_value", 123)));
                  THEN("object check")
                  {
                        auto obj = json::get(doc, "obj_value");
                        REQUIRE(json::equals(json::get(obj, "int_value"), 123));
                  }

                  THEN("stringified")
                  {
                        REQUIRE(json::stringify(doc) == "{\"obj_value\":{\"int_value\":123}}");
                  }
            }

            WHEN("double value added")
            {
                  auto doc = json::document(json::float64("d_value", 123.45));
                  THEN("double value check")
                  {
                        REQUIRE(json::equals(json::get(doc, "d_value"), 123.45));
                  }

                  THEN("stringified")
                  {
                        REQUIRE(json::stringify(doc) == "{\"d_value\":123.45}");
                  }
            }

      }
}
