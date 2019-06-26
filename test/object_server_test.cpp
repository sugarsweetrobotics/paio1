#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <paio/obj/json.h>
#include <paio/obj/object_server.h>

using namespace paio;

struct TM2
{                  // TestMessage
    std::string h; // header
    std::string b; // body

    TM2& operator=(const TM2& tm) {
        h = tm.h;
        b = tm.b;
        return *this;
    }
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

auto containize = +[](const paio::json::Container &c) {
    return TM2({json::string(c, "h"), json::string(c, "b")});
};

SCENARIO("Jsonizer", "[object]")
{
    GIVEN("Object JSONizer")
    {
        auto obj = paio::object<TM2>({"header", "body"});
        auto jobj = obj >>= jsonize;
        REQUIRE(json::string(json::get(jobj, "__className__")) == typeid(TM2).name());
        REQUIRE(json::string(json::get(jobj, "h")) == "header");
        REQUIRE(json::string(json::get(jobj, "b")) == "body");
    }
}

SCENARIO("Object Server", "[object]")
{
    GIVEN("Object Server insert without error")
    {
        const int port = 9559;
        auto os1 = paio::object_server();
        auto obj = paio::object<TM2>({"header", "body"});
        REQUIRE((obj >>= +[](const TM2 &tm) { return (tm.h == "header"); }));
        REQUIRE((obj >>= +[](const TM2 &tm) { return (tm.b == "body"); }));

        auto os2 = paio::registerObject(std::move(os1), "/topic1", obj, jsonize, containize);
        auto os = paio::startUnlock(std::move(os2), "localhost", port);

        THEN("Can access to server")
        {
            auto r = http::get("localhost", port, "/topic1");

            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body\",\"h\":\"header\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
        }

        THEN("Can put data to server")
        {
            auto obj2 = paio::object<TM2>({"header2", "body2"});
            auto os4 = paio::put(std::move(os), "/topic1", obj2);

            auto r = http::get("localhost", port, "/topic1");
            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
        }

        THEN("Can write data through server")
        {
            http::put("localhost", port, "/topic1", std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}", "application/json");

            auto r = http::get("localhost", port, "/topic1");
            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
        }
    }
}

SCENARIO("Object Server Access TM2", "[object]")
{
    GIVEN("Object Server insert without error")
    {
        const int port = 9559;
        auto os1 = paio::object_server();
        auto obj = paio::object<TM2>({"header", "body"});
        REQUIRE((obj >>= +[](const TM2 &tm) { return (tm.h == "header"); }));
        REQUIRE((obj >>= +[](const TM2 &tm) { return (tm.b == "body"); }));

        auto os2 = paio::registerObject(std::move(os1), "/topic1", obj, jsonize, containize);
        auto os = paio::startUnlock(std::move(os2), "localhost", port);

        auto con = paio::containizer(containize);
        THEN("Can read data through server")
        {
            auto obj2 = con(paio::json::parse(http::get("localhost", port, "/topic1").body));
            REQUIRE((*(obj.get())) == ((paio::get<TM2>(obj2))));
        }
    }
}
