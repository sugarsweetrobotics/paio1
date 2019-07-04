#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <sstream>
#include <paio/proc/process.h>

struct MSG {
  std::string body;
};

const std::string string_name = paio::demangle_name(typeid(std::string).name());
//"std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >";
      
SCENARIO("Process Basic Test", "[process]")
{

  GIVEN("Two arg function Process") {
    auto functionPointer = std::function<std::tuple<std::string, std::string>(const std::string&)>(
      [](const std::string& c) {
        return std::make_tuple("Name is " + c, "Name was " + c);
      });
    auto processFunction01 = paio::processFunction(functionPointer);

    THEN("Argument TypeName") {
      auto typeNames = processFunction01->argumentTypeNames();
      REQUIRE(typeNames[0] == string_name);
    }

    THEN("Return TypeName") {
      auto typeNames = processFunction01->returnTypeNames();
      REQUIRE(typeNames[0] == string_name);
      REQUIRE(typeNames[1] == string_name);
    }
  }

  
  GIVEN("Two arg function Process") {
    auto functionPointer = std::function<std::string(unsigned int,const std::string&)>(
      [](unsigned int i, const std::string& c) { 
        std::ostringstream oss;
        oss << "Number is " << i << ", String is " << c;
        return oss.str();
      });
    auto processFunction01 = new paio::ProcessFunction<std::string, unsigned int, const std::string&>(functionPointer);

    THEN("Argument TypeName") {
      auto typeNames = processFunction01->argumentTypeNames();
      REQUIRE(typeNames[0] == "unsigned int");
      REQUIRE(typeNames[1] == string_name);
    }

    THEN("Bind first argument") {
      auto processFunction02 = processFunction01->bind(3);
      REQUIRE(!!processFunction02);
      auto typeNames = processFunction02->argumentTypeNames();
      REQUIRE(typeNames[0] == string_name);
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

    THEN("Return TypeName") {
      auto typeNames = processFunction01->returnTypeNames();
      REQUIRE(typeNames[0] == string_name);
    }
  }

  GIVEN("Simple Function") {
    std::function<std::string(const std::string&)> fp = [](const std::string& c) { 
        std::ostringstream oss;
        oss << "String is " << c;
        return oss.str();
      };
    auto processFunction01 = paio::processFunction(fp);

    THEN("Argument TypeName") {
      auto typeNames = processFunction01->argumentTypeNames();
      REQUIRE(typeNames[0] == string_name);
    }

    THEN("Return TypeName") {
      auto typeNames = processFunction01->returnTypeNames();
      REQUIRE(typeNames[0] == string_name);
    }

    THEN("Can call") {
      auto retval = processFunction01->call("hoge");
      REQUIRE(retval.value() == "String is hoge");
    }

    THEN("Can not bind all argument.") {
      auto processFunction02 = processFunction01->bind("hoo");
      REQUIRE(processFunction02 == nullptr);
    }
  }
}