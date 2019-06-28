#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <paio/obj/json.h>
#include <paio/obj/object_server.h>

using namespace paio;

struct TM2
{                  // TestMessage
    std::string h; // header
    std::string b; // body

    TM2 &operator=(const TM2 &tm)
    {
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

SCENARIO("Object Server Callbacks", "[object]")
{
    GIVEN("Object Server insert without error")
    {
        const int port = 9559;
        auto os1 = paio::object_server();
        auto obj = paio::object<TM2>({"header", "body"});
        REQUIRE((obj >>= +[](const TM2 &tm) { return (tm.h == "header"); }));
        REQUIRE((obj >>= +[](const TM2 &tm) { return (tm.b == "body"); }));

        auto os2 = paio::registerObject("topic1", *(obj.get()), jsonize, containize)(std::move(os1));
        auto os = paio::startUnlock("localhost", port)(std::move(os2));

        THEN("GET initiates callback")
        {
            int dcount = 0;
            paio::OnJsonRead f = [&dcount](paio::json::Container &&jc) {
                dcount++;
                return jc;
            };
            os = paio::registerOnJsonRead("topic1", f)(std::move(os));
            auto r = http::get("localhost", port, "/topic/topic1");

            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body\",\"h\":\"header\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
            REQUIRE(dcount == 1);
        }

        THEN("Can put data to server initiates callback")
        {
            int dcount = 0;
            paio::OnJsonWrite f = [&dcount](paio::json::Container &&jc) {
                dcount++;
                std::cout << "onJsonWrite:" << paio::json::stringify(jc) << std::endl;
                return std::move(jc);
            };
            os = paio::registerOnJsonWrite("topic1", f)(std::move(os));

            http::put("localhost", port, "/topic/topic1", std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}", "application/json");

            auto r = http::get("localhost", port, "/topic/topic1");
            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
            REQUIRE(dcount == 1);
        }

        THEN("Can directly put data to server initiates callback")
        {
            int dcount = 0;
            paio::OnJsonWrite f = [&dcount](paio::json::Container &&jc) {
                dcount++;
                return std::move(jc);
            };

            int count = 0;
            paio::OnPutObjectContainer f2 = [&count](paio::ObjectContainer &&jc) {
                count++;
                return std::move(jc);
            };

            os = paio::registerOnJsonWrite("topic1", f)(std::move(os));
            os = paio::registerOnPutObjectContainer("topic1", f2)(std::move(os));

            auto obj2 = paio::object<TM2>({"header2", "body2"});
            auto os4 = paio::registerObject("topic1", *(obj2.get()))(std::move(os));

            auto r = http::get("localhost", port, "/topic/topic1");
            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
            REQUIRE(dcount == 0);
            REQUIRE(count == 1);
        }

    }
}

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

        auto os2 = paio::registerObject("topic1", *(obj.get()), jsonize, containize)(std::move(os1));
        auto os = paio::startUnlock("localhost", port)(std::move(os2));

        THEN("Can access to server")
        {
            auto r = http::get("localhost", port, "/topic/topic1");

            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body\",\"h\":\"header\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
        }

        THEN("Can put data to server")
        {
            auto obj2 = paio::object<TM2>({"header2", "body2"});
            auto os4 = paio::registerObject("topic1", *(obj2.get()))(std::move(os));
            auto r = http::get("localhost", port, "/topic/topic1");
            REQUIRE(r.status == 200);
            REQUIRE(r.body == std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}");
        }

        THEN("Can write data through server")
        {
            http::put("localhost", port, "/topic/topic1", std::string("{\"b\":\"body2\",\"h\":\"header2\",\"__className__\":\"") + typeid(TM2).name() + "\"}", "application/json");

            auto r = http::get("localhost", port, "/topic/topic1");
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

        auto os2 = paio::registerObject("topic1", *(obj.get()), jsonize, containize)(std::move(os1));
        auto os = paio::startUnlock("localhost", port)(std::move(os2));

        auto con = paio::containizer(containize);
        THEN("Can read data through server")
        {
            auto obj2 = con.value()(paio::json::parse(http::get("localhost", port, "/topic/topic1").body));
            REQUIRE((*(obj.get())) == ((paio::get<TM2>(obj2))));
        }
    }
}
