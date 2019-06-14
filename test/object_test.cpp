#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <paio/obj/object.h>

using namespace paio;

struct TM { // TestMessage
  std::string h; // header
  std::string b; // body
};

bool operator==(const TM& m1, const TM& m2) {
  return (m1.h == m2.h) && (m1.b == m2.b);
}


SCENARIO( "Object", "[object]" ) {

  GIVEN("Object Can Comparable") {
    auto obj = paio::object<TM>({"header", "body"});

    REQUIRE( paio::get<TM>(obj)->h == "header" );
    REQUIRE( paio::get<TM>(obj)->b == "body" );


    THEN("Monoid Rule 1") {
      std::function<bool(const TM&)> f = [](const TM& tm) {
	return tm.h == "header" && tm.b == "body";
      };
      const TM x({"header", "body"});
      REQUIRE( (paio::object<TM>(x) >>= f) == f(x) );
    }

    THEN("Monad Rule 2") {
      auto m = paio::object<TM>({"header", "body"});
      std::function<Object<TM>(const TM&)> retn = paio::retn<TM>;
      REQUIRE( (m >>= retn) == m );
    }

    THEN("Monad Rule 3") {
      auto m = paio::object<TM>({"header", "body"});
      // f and g must be static function to use lambda function without lambda-capture
      static const std::function<Object<TM>(const TM&)> f = [](const TM& tm) {
	return Object<TM>({tm.h + "_foo", tm.b + "_foo"});
      };
      static const std::function<Object<TM>(const TM&)> g = [](const TM& tm) {
	return Object<TM>({tm.h + "_goo", tm.b + "_goo"});
      };
      
      REQUIRE( ((m >>= f) >>= g) == (m >>= +[](const TM& x) {
	    return f(x) >>= g;
	  }) );

      // if you need to use lambda-capture, use bind member function with template argument
      // or use std::function object
      std::function<Object<TM>(const TM&)> h = [](const TM& tm) {
	return Object<TM>({tm.h + "_hoo", tm.b + "_hoo"});
      };
      std::function<Object<TM>(const TM&)> t = [](const TM& tm) {
	return Object<TM>({tm.h + "_too", tm.b + "_too"});
      };

      REQUIRE( ((m >>= h) >>= t) == (m.bind<Object<TM>>([h, t](const TM& x) {
	    return h(x) >>= t;
	    }) ) );

      std::function<Object<TM>(const TM&)> lambda = [h, t](const TM& x) { 
	return h(x) >>= t;
      };
      
      REQUIRE( ((m >>= h) >>= t) == (m >>= lambda) );
    }
  }

  GIVEN("Object Can Manipulate") {
    auto obj = paio::object<TM>({"header", "body"});

    REQUIRE( paio::get<TM>(obj)->h == "header" );
    REQUIRE( paio::get<TM>(obj)->b == "body" );

    THEN("Change with monoid") {
      obj.does(
	       [](TM&& value) {
		 REQUIRE( value.h == "header" );
		 REQUIRE( value.b == "body" );
		 value.h = "header2";
		 value.b = "body2"; 
		 return value;
	       },

	       [](TM&& value) {
		 REQUIRE( value.h == "header2" );
		 REQUIRE( value.b == "body2" );

		 return value;
	       }
	       
	       );
    }
    
  }


  GIVEN("Object Can Create") {
    auto obj = paio::object<TM>({"header", "body"});

    REQUIRE( paio::get<TM>(obj)->h == "header" );
    REQUIRE( paio::get<TM>(obj)->b == "body" );

    THEN("Can access as monoid") {
      obj.does(
	       [](TM&& value) {
		 REQUIRE( value.h == "header" );
		 REQUIRE( value.b == "body" );
		 return value;
	       }
	       );

    }

    THEN("Can access as monoid twice") {
      obj.does(
	       [](TM&& value) {
		 REQUIRE( value.h == "header" );
		 REQUIRE( value.b == "body" );
		 return value;
	       }
	       );

      obj.does(
	       [](TM&& value) {
		 REQUIRE( value.h == "header" );
		 REQUIRE( value.b == "body" );
		 return value;
	       }
	       );
    }


  }
  

  /*
  GIVEN("With Server") {
    int port = 9559;
    auto s = http::server("localhost", port);
    s = http::serve("/", "GET", [&](http::Request&& r) {
	return http::Response(200, "Hello");
      }, std::move(s));
    
    s = http::listen(1.0, std::move(s));
    
    THEN("Can access to server") {
      auto r = http::get("localhost", port, "/");
      REQUIRE(r.status == 200);
      REQUIRE(r.body == "Hello");
    }
  }
  
  GIVEN("With Server (functional style)") {
    int port = 9557;
    auto t = paio::compose<http::Server_ptr>(http::listen(1.0),
					     http::serve("/", "GET", [&](http::Request&& r) {
						 return http::Response(200, "Hello");
					       }));
    auto s = t(http::server("localhost", port));
			   
    THEN("Can access to server") {
      auto r = http::get("localhost", port, "/");
      REQUIRE(r.status == 200);
      REQUIRE(r.body == "Hello");
    }
  }
  
  
  /*
    GIVEN("With Server") {
    WHEN("Server gives hello") {
      int port = 9555;
      auto s = http::server("localhost", port);
      REQUIRE(s->address == "localhost");
      REQUIRE(s->port == port);
    
      s = http::serve(std::move(s), "/", "GET", [&](http::Request&& r) {
	  return http::Response(200, "Hello");
	});
      s = http::listen(std::move(s), 1.0);
      THEN("Get Hello Message") {
	auto r = http::get("localhost", port, "/");
	REQUIRE(r.status == 200);
	REQUIRE(r.body == "Hello");
      }
    }


    WHEN("Server requests Hello") {
      int port = 9556;
      auto s = http::server("localhost", port);
      REQUIRE(s->address == "localhost");
      REQUIRE(s->port == port);
    
      s = http::serve(std::move(s), "/", "PUT", [&](http::Request&& r) {
	  return http::Response(200, r.body + " World");
	});
      s = http::listen(std::move(s), 1.0);

      THEN("Put Hello Message") {
	auto r = http::put("localhost", port, "/", "Hello", "text/plain");
	REQUIRE(r.status == 200);
	REQUIRE(r.body == "Hello World");
      }
      }*/
    
    /*
    WHEN("Server binds Dynamic Port") {
      int port = 0;
      auto s = http::server("localhost", port);
      REQUIRE(s->address == "localhost");
      REQUIRE(s->port == port);
    
      http::serve(s, "/", "GET", [&](http::Request&& r) {
	  return http::Response(200, "Hello");
	});
      port = http::listen(s, 1.0);
      std::cout << "port is " << port << std::endl;
      THEN("Get Hello Message") {
	auto r = http::get("localhost", port, "/");
	REQUIRE(r.status == 200);
	REQUIRE(r.body == "Hello");
      }
      }*/

  //  }

}


