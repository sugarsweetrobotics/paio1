#pragma once

//#include <paio/paio.h>
#include <vector>

#include <stdint.h>
#include <string>
#include <exception>

namespace paio
{

namespace json
{

class ParseException : public std::exception
{
};

class Document;
using Document_ptr = std::shared_ptr<Document>;
struct Document
{
      virtual ~Document() {}
};

struct Object;
using Object_ptr = std::shared_ptr<Object>;

struct Object
{
      virtual ~Object() {}
      virtual Object_ptr get(int32_t index) const = 0;
};

struct Container
{
      Document_ptr _d_ptr;
      Object_ptr _ptr;

      Container() : _ptr(nullptr), _d_ptr(nullptr) {}

      Container(Object_ptr &&p) : _ptr(p), _d_ptr(nullptr) {}

      Container(Document_ptr &&p) : _d_ptr(p), _ptr(nullptr) {}

      Container(Container &&c) : _ptr(c._ptr), _d_ptr(c._d_ptr) {}



      Container(const Container &c)
      {
            _copyFrom(c);
      }
      Container &operator=(const Container &c)
      {
            _copyFrom(c);
            return *this;
      }

      void _copyFrom(const Container &c)
      {
            _ptr = c._ptr;
            _d_ptr = c._d_ptr;
      }
      
      Container operator[](int i)
      {
            if (_ptr)
            {
                  return Container(_ptr->get(i));
            }
            else
            {
                  return Container(Object_ptr(nullptr));
            }
      }
};

Container parse(const std::string &s);
//      Container get(const Document_ptr& j, std::string&& label);
std::vector<std::string> keys(Container &doc);

Container get(const Container &j, const std::string &label);

std::string string(const Container &v);
int32_t int32(const Container &v);
uint32_t uint32(const Container &v);
float float32(const Container &v);
double float64(const Container &v);

template <typename T>
T value(const Container &v) { return int32(v); }

template <>
inline std::string value<std::string>(const Container &v) { return string(v); }

template <>
inline uint32_t value<uint32_t>(const Container &v) { return uint32(v); }

template <>
inline int32_t value<int32_t>(const Container &v) { return int32(v); }

template <>
inline float value<float>(const Container &v) { return float32(v); }

template <>
inline double value<double>(const Container &v) { return float64(v); }

inline bool is_nil(const Container &v) { return v._ptr == nullptr && v._d_ptr == nullptr; }

/*
      inline bool equals(const Container& c, uint32_t v) {
	return (!is_nil(c) && uint32(c) == v);
	}*/

template <typename T>
inline bool equals(const Container &c, const T v)
{
      return (!is_nil(c) && value<T>(c) == v);
}

template <>
inline bool equals(const Container &c, const char *v)
{
      return (!is_nil(c) && value<std::string>(c) == v);
}

/*
      inline bool equals(const Container& c, const double& v) {
	return (!is_nil(c) && float64(c) == v);
	}*/

inline std::string string(const Container &v, std::string &&label)
{
      return string(get(v, std::move(label)));
}

inline std::uint32_t uint32(const Container &v, std::string &&label)
{
      return uint32(get(v, std::move(label)));
}

struct KeyValue
{
      std::string key;
      KeyValue(std::string &&key) : key(key) {}
      KeyValue(const std::string &key) : key(key) {}
      virtual ~KeyValue() {}
};

using KeyValue_ptr = std::shared_ptr<KeyValue>;
using Allocator = std::function<KeyValue_ptr(Document_ptr &)>;

Container registerDocument(Container &&d, Allocator f);
Container document();

template <typename First, typename... Rest>
Container document(const First &fst, const Rest &... rest)
{
      //return registerContent(document(rest...), fst);
      return registerDocument(document(rest...), fst);
}

Allocator object(std::string &&label);

Allocator registerContainer(Allocator o, Allocator f);

template <typename First, typename... Rest>
Allocator object(std::string &&label, const First &fst, const Rest &... rest)
{
      //return registerContent(document(rest...), fst);
      return registerContainer(object(std::move(label), rest...), fst);
}

KeyValue_ptr int32_(std::string &&label, int32_t value);
KeyValue_ptr uint32_(std::string &&label, uint32_t value);
KeyValue_ptr string_(std::string &&label, const char *value, Document_ptr &doc);
KeyValue_ptr float32_(std::string &&label, float value);
KeyValue_ptr float64_(std::string &&label, double value);

inline Allocator int32(std::string &&label, int32_t value)
{
      return [&](Document_ptr &d) { return int32_(std::move(label), value); };
}

inline Allocator float64(std::string &&label, double value)
{
      return [&](Document_ptr &d) { return float64_(std::move(label), value); };
}

inline Allocator string(std::string &&label, std::string &&value)
{
      return [&](Document_ptr &d) { return string_(std::move(label), value.c_str(), d); };
}

inline Allocator string(std::string &&label, const std::string &value)
{
      return [&](Document_ptr &d) { return string_(std::move(label), value.c_str(), d); };
}

std::string stringify(const Container &d);

template <typename T>
std::vector<T> map(Container &doc, std::function<T(Container)> f)
{
      std::vector<T> v;
      auto ks = keys(doc);
      for (auto &k : ks)
      {
            v.push_back(f(get(doc, std::move(k))));
      }
      return v;
}
}; // namespace json

}; // namespace paio
