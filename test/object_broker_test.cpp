#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <functional>
#include <paio/object_broker.h>

using namespace paio;

struct TM
{                  // TestMessage
    std::string h; // header
    std::string b; // body

    TM& operator=(const TM& tm) {
        h = tm.h;
        b = tm.b;
        return *this;
    }
};

bool operator==(const TM &m1, const TM &m2)
{
    return (m1.h == m2.h) && (m1.b == m2.b);
}

auto jsonize = +[](const TM &tm) {
    return paio::json::document(
        json::string("__className__", typeid(TM).name()),
        json::string("h", tm.h),
        json::string("b", tm.b));
};

auto containize = +[](const paio::json::Container &c) {
    return TM({json::string(c, "h"), json::string(c, "b")});
};


SCENARIO("Object Broker test", "[objbroker]")
{
    GIVEN("Broker create")
    {
        auto port = 9559;
        auto obj = paio::object<TM>({"header", "body"});
        auto ob = paio::object_broker();
        ob.does(
            paio::registerObject<TM>("topic1", {"header", "body"}, jsonize, containize),
            paio::startUnlock("localhost", port)
        );
        
        THEN("Can access to server")
        {
            auto r = http::get("localhost", port, "/topic/topic1");

            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body\",\"h\":\"header\",\"__className__\":\"") + typeid(TM).name() + "\"}");
        }

        THEN("Can put to server")
        {
            auto r1 = http::put("localhost", port, "/topic/topic1", paio::json::stringify(jsonize(TM({"header2", "body2"}))), "application/json");

            REQUIRE(r1.status == 200);
            auto r = http::get("localhost", port, "/topic/topic1");

            
            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM).name() + "\"}");
        }
    }
}