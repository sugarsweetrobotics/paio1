#pragma once

#include <string>
#include <vector>
#include <regex>

namespace paio
{

namespace http
{

struct Header
{
  std::string key;
  std::string value;
};

struct Request
{
  std::string version;
  std::string method;
  std::string target;
  std::string path;
  std::vector<Header> headers;
  std::string body;
  std::smatch matches;
  void* _privateData;
  Request(const std::string &m, const std::string &b) : method(m), body(b), _privateData(nullptr) {}
  Request(std::string &&m, std::string &&b) : method(m), body(b), _privateData(nullptr) {}
  Request(const std::string &m, const std::string &b, const std::smatch &ms, void* privateData=nullptr) :
   method(m), body(b), matches(ms), _privateData(privateData) {}
  Request(std::string &&m, std::string &&b, std::smatch&& ms) : method(m), body(b), matches(ms), _privateData(nullptr) {}
};

struct Response
{
  std::string version;
  int32_t status;
  std::vector<Header> headers;
  std::string body;
  Response() : version("1.0"), status(0) {}
  Response(int32_t s) : version("1.0"), status(s), body("") {}
  Response(int32_t s, std::string &&body) : version("1.0"), status(s), body(body) {}
  Response(int32_t s, const std::string &body) : version("1.0"), status(s), body(body) {}
  Response(Response &&r) : version(r.version), status(r.status), body(r.body) {}
};

//Response get(const std::string& url, const Request& request);
Response get(std::string &&host, uint16_t port, std::string &&pattern);
Response put(const std::string &host, uint16_t port, const std::string &pattern, const std::string &body, const std::string &content_type);
Response get(const std::string &url, const Request &request);
Response put(const std::string &url, const Request &request);
Response post(const std::string &url, const Request &request);
}; // namespace http

}; // namespace paio
