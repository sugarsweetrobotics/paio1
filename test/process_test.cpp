#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <sstream>
#include <paio/process.h>

struct MSG {
  std::string body;
};

template<typename F, typename T, std::size_t I>
void
apply_impl(F f, T const& t){
    f(std::get<I>(t));
}

template<typename F, typename T, std::size_t ...Indices>
void
apply_impl(F f, T const& t, std::size_t n, std::index_sequence<Indices...>){
    ((void (*[sizeof...(Indices)])(F, T const&)){apply_impl<F, T, Indices>...})[n](f, t);
}


template<typename F, typename ...Args>
void apply(F f, std::tuple<Args...> const& t, std::size_t n){
    apply_impl(f, t, n, std::index_sequence_for<Args...>());
}

template<typename F, typename ...Args>
void for_each(std::tuple<Args...> const& t, F f){
    for(std::size_t i = 0 ; i < sizeof...(Args) ; ++i){
        apply(f, t, i);
    }
}
const std::string string_name = paio::demangle_name(typeid(std::string).name());
//"std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >";
      
SCENARIO("Process Basic Test", "[process]")
{
  GIVEN("Two arg function Process") {
    auto functionPointer = std::function<std::string(unsigned int,const std::string&)>(
      [](unsigned int i, const std::string& c) { 
        std::ostringstream oss;
        oss << "Number is " << i << ", String is " << c;
        return oss.str();
      });
    auto processFunction01 = new paio::ProcessFunction<std::string, unsigned int, const std::string&>(functionPointer);

    THEN("TypeName") {
      auto typeNames = processFunction01->typeName();
      REQUIRE(std::get<0>(typeNames) == "unsigned int");
      REQUIRE(std::get<1>(typeNames) == string_name);
    }

    THEN("Bind first argument") {
      auto processFunction02 = processFunction01->bind(3);
      REQUIRE(!!processFunction02);
      auto typeNames = processFunction02->typeName();
      REQUIRE(std::get<0>(typeNames) == string_name);
    }

    THEN("Can not bind all argument.") {
      auto processFunction02 = processFunction01->bind(3);
      auto processFunction03 = processFunction02->bind("hoo");
      REQUIRE(processFunction03 == nullptr);
    }

    THEN("Can not bind all argument. Last argument must be passed with call function") {
      auto processFunction02 = processFunction01->bind(3);
      auto return_value = paio::call<std::string, const std::string&>(processFunction02, std::string("hoo"));
      REQUIRE(return_value.value() == "Number is 3, String is hoo");
    }

    THEN("Call any function") {
      auto processFunction02 = processFunction01->bind(3);
      auto hoo = (std::shared_ptr<void>(new std::string("hoo")));
      auto return_value = processFunction02->callAny(hoo);
      REQUIRE(!!return_value);
      auto ret = std::static_pointer_cast<std::string>(return_value.value());
      REQUIRE(*ret == "Number is 3, String is hoo");
      
    }

    THEN("Call any function") {
      auto three = std::shared_ptr<void>(new unsigned int(3));
      auto processFunction02 = processFunction01->bindAny(three);
      auto hoo = (std::shared_ptr<void>(new std::string("hoo")));
      auto return_value = processFunction02->callAny(hoo);
      REQUIRE(!!return_value);
      auto ret = std::static_pointer_cast<std::string>(return_value.value());
      REQUIRE(*ret == "Number is 3, String is hoo");
    }

  }

  GIVEN("Simple Function") {
    /*
    std::function<std::string(std::string)> fp = [](std::string msg) {
      return msg + " is hoge.";
    };
    auto pf = new paio::ProcessFunction<std::string, std::string>(fp);

  }

  GIVEN("Simple Function") {
    std::function<std::string(std::string, std::string)> fp = [](std::string msg, std::string msg2) {
      return msg + " is hoge." + msg2 + " is hage.";
    };
    auto pf = new paio::ProcessFunction<std::string, std::string, std::string>(fp);

  }
  */
  }
}