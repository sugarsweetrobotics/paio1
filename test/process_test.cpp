#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>


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

SCENARIO("Process Basic Test", "[process]")
{
  GIVEN("Null Process") {
    auto fp = std::function<std::string(unsigned int,const std::string&)>([](unsigned int i, const std::string& c) { return c + " is m[" + std::string("hoo") + "]"; });
    auto p = new paio::ProcessFunction<std::string, unsigned int, const std::string&>(fp);
    auto t = p->typeName();
    for(auto s : t) {
      std::cout << ":" << s << ", " << std::endl;
    }

    auto p2 = new paio::ProcessFunction<std::string, const std::string&>(p->bind(3));
    std::cout << p->bindType(3)->bind("hoge")() << std::endl;
    
    auto t2 = p2->typeName();
    for(auto s : t2) {
      std::cout << ":" << s << ", " << std::endl;
    }

    /*
    for_each(t, [](auto it){
        std::cout << it + it << std::endl;
    }); */
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