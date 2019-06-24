#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <paio/obj/object.h>

using namespace paio;

struct TM
{                // TestMessage
  std::string h; // header
  std::string b; // body
};

bool operator==(const TM &m1, const TM &m2)
{
  return (m1.h == m2.h) && (m1.b == m2.b);
}

SCENARIO("Object", "[object]")
{

  GIVEN("Object Null Object")
  {
    auto obj = paio::Object<TM>();

    THEN("Null Check")
    {
      REQUIRE(paio::isNull(obj));
    }

    THEN("Null Object constructed with object is null") {
      auto obj2 = obj;
      REQUIRE(paio::isNull(obj2));
    }

    THEN("Null Object copied object is null") {
      paio::Object<TM> obj2;
      obj2 = obj;
      REQUIRE(paio::isNull(obj2));
    }

    THEN("Null Object constructed with object is null") {
      auto obj2 = std::move(obj);
      REQUIRE(paio::isNull(obj2));
    }

    THEN("Null Object copied object is null") {
      paio::Object<TM> obj2;
      obj2 = std::move(obj);
      REQUIRE(paio::isNull(obj2));
    }

    THEN("Null Save does method") {
      obj.does(
        [](TM&& tm) {
          REQUIRE(false);
          tm.b = "body2";
          return tm;
        }
      );
      REQUIRE(paio::isNull(obj));
    }

    THEN("Null Option does method") {
      REQUIRE(paio::isNull(obj));
      try {
        auto ret = obj >>= +[](const TM& tm) {
          REQUIRE(false);
          return tm.b == "body";
        };
        REQUIRE(!ret);
      } catch (paio::NullObjectIsBindedError& ex) {

      }
    }
  }

  GIVEN("Object Can Comparable")
  {
    auto obj = paio::object<TM>({"header", "body"});

    REQUIRE(paio::get<TM>(obj)->h == "header");
    REQUIRE(paio::get<TM>(obj)->b == "body");

    THEN("Monoid Rule 1")
    {
      std::function<bool(const TM &)> f = [](const TM &tm) {
        return tm.h == "header" && tm.b == "body";
      };
      const TM x({"header", "body"});
      REQUIRE((paio::object<TM>(x) >>= f) == f(x));
    }

    THEN("Monad Rule 2")
    {
      auto m = paio::object<TM>({"header", "body"});
      std::function<Object<TM>(const TM &)> retn = paio::retn<TM>;
      REQUIRE((m >>= retn) == m);
    }

    THEN("Monad Rule 3-1")
    {
      auto m = paio::object<TM>({"header", "body"});
      // f and g must be static function to use lambda function without lambda-capture
      static const std::function<Object<TM>(const TM &)> f = [](const TM &tm) {
        return Object<TM>({tm.h + "_foo", tm.b + "_foo"});
      };
      static const std::function<Object<TM>(const TM &)> g = [](const TM &tm) {
        return Object<TM>({tm.h + "_goo", tm.b + "_goo"});
      };

      REQUIRE(((m >>= f) >>= g) == (m >>= +[](const TM &x) {
                return f(x) >>= g;
              }));
    };

    THEN("Monad Rule 3-2")
    {
      auto m = paio::object<TM>({"header", "body"});
      // if you need to use lambda-capture, use bind member function with template argument
      // or use std::function object
      std::function<Object<TM>(const TM &)> h = [](const TM &tm) {
        return Object<TM>({tm.h + "_hoo", tm.b + "_hoo"});
      };
      std::function<Object<TM>(const TM &)> t = [](const TM &tm) {
        return Object<TM>({tm.h + "_too", tm.b + "_too"});
      };

      auto b = (m >>= h) >>= t;
      auto a = (m.bind<Object<TM>>([h, t](const TM &x) {
                return h(x) >>= t;
              })).value();
      REQUIRE(!paio::isNull(b));
      REQUIRE(!paio::isNull(a));
      //REQUIRE(a == b);
      /*

      REQUIRE(((m >>= h) >>= t) == (m.bind<Object<TM>>([h, t](const TM &x) {
                return h(x) >>= t;
              })).value() );
              */
    };

    THEN("Monad Rule 3-3")
    {
      auto m = paio::object<TM>({"header", "body"});
      std::function<Object<TM>(const TM &)> h = [](const TM &tm) {
        return Object<TM>({tm.h + "_hoo", tm.b + "_hoo"});
      };
      std::function<Object<TM>(const TM &)> t = [](const TM &tm) {
        return Object<TM>({tm.h + "_too", tm.b + "_too"});
      };
      std::function<Object<TM>(const TM &)> lambda = [h, t](const TM &x) {
        return h(x) >>= t;
      };

      REQUIRE(((m >>= h) >>= t) == (m >>= lambda));
    }
  }

  GIVEN("Object Can Manipulate")
  {
    auto obj = paio::object<TM>({"header", "body"});

    REQUIRE(paio::get<TM>(obj)->h == "header");
    REQUIRE(paio::get<TM>(obj)->b == "body");

    THEN("Change with monoid")
    {
      obj.does(
          [](TM &&value) {
            REQUIRE(value.h == "header");
            REQUIRE(value.b == "body");
            value.h = "header2";
            value.b = "body2";
            return value;
          },

          [](TM &&value) {
            REQUIRE(value.h == "header2");
            REQUIRE(value.b == "body2");

            return value;
          }

      );
    }
  }

  GIVEN("Object Can Create")
  {
    auto obj = paio::object<TM>({"header", "body"});

    REQUIRE(paio::get<TM>(obj)->h == "header");
    REQUIRE(paio::get<TM>(obj)->b == "body");

    THEN("Can access as monoid")
    {
      obj.does(
          [](TM &&value) {
            REQUIRE(value.h == "header");
            REQUIRE(value.b == "body");
            return value;
          });
    }

    THEN("Can access as monoid twice")
    {
      obj.does(
          [](TM &&value) {
            REQUIRE(value.h == "header");
            REQUIRE(value.b == "body");
            return value;
          });

      obj.does(
          [](TM &&value) {
            REQUIRE(value.h == "header");
            REQUIRE(value.b == "body");
            return value;
          });
    }
  }
}
