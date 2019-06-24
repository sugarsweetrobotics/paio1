#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <paio/obj/json.h>
#include <paio/object_server.h>

using namespace paio;

struct TM2
{                  // TestMessage
    std::string h; // header
    std::string b; // body
};

bool operator==(const TM2 &m1, const TM2 &m2)
{
    return (m1.h == m2.h) && (m1.b == m2.b);
}

auto jsonize = +[](const TM2 &tm) {
    return paio::json::document(
        json::string("__className__", typeid(TM2).name()),
        json::string("h", tm.h),
        json::string("b", tm.b));
};

auto containize = +[](const paio::json::Container& c) {
    return paio::Object<TM2>({json::string(c, "h"), json::string(c, "b")});
};

SCENARIO("Object Server", "[object]")
{
    GIVEN("Object JSONizer")
    {
        auto obj = paio::object<TM2>({"header", "body"});
        auto jobj = obj >>= jsonize;
        REQUIRE(json::string(json::get(jobj, "__className__")) == typeid(TM2).name());
        REQUIRE(json::string(json::get(jobj, "h")) == "header");
        REQUIRE(json::string(json::get(jobj, "b")) == "body");
    }

    GIVEN("Object Server insert without error")
    {
        const int port = 9559;
        auto os = paio::object_server();
        auto obj = paio::object<TM2>({"header", "body"});
        REQUIRE((obj >>= +[](const TM2 &tm) { return (tm.h == "header"); }));
        REQUIRE((obj >>= +[](const TM2 &tm) { return (tm.b == "body"); }));

        os = paio::registerObject(std::move(os), "/topic1", obj, jsonize);
        os = paio::startUnlock(std::move(os), "localhost", port);
        THEN("Can access to server")
        {
            auto r = http::get("localhost", port, "/topic1");
            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body\",\"h\":\"header\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
        }

        THEN("Can put data to server")
        {
            auto obj2 = paio::object<TM2>({"header2", "body2"});
            os = paio::put(std::move(os), "/topic1", obj2);

            auto r = http::get("localhost", port, "/topic1");
            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}");        
        }
    }
}