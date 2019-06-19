#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <paio/obj/json.h>
#include <paio/object_server.h>

using namespace paio;

struct TM
{                  // TestMessage
    std::string h; // header
    std::string b; // body
};

bool operator==(const TM &m1, const TM &m2)
{
    return (m1.h == m2.h) && (m1.b == m2.b);
}

auto jsonize = +[](const TM &tm) {
    return paio::json::document(
        json::string("__className__", "TM"),
                     json::string("h", tm.h),
                     json::string("b", tm.b));
};

SCENARIO("Object Server", "[object]")
{
    GIVEN("Object JSONizer")
    {
        auto obj = paio::object<TM>({"header", "body"});
        auto jobj = obj >>= jsonize;
        REQUIRE(json::string(json::get(jobj.value(), "__className__")) == "TM");
    }

    GIVEN("Object Server insert without error")
    {
        auto os = paio::object_server();

        auto obj = paio::object<TM>({"header", "body"});
        REQUIRE(paio::get<TM>(obj)->h == "header");
        REQUIRE(paio::get<TM>(obj)->b == "body");

        os = paio::registerObject(std::move(os), "topic1", obj);

        os = paio::startUnlock(std::move(os), "0.0.0.0", 9559);
    }


}