
#include <utility>

#include <paio/http/server.h>
#include <iostream>
#include <httplib.h>

using namespace paio;

http::Request convert(const httplib::Request &req)
{
  return http::Request(req.method, req.body, req.matches);
}


http::Request convert(const httplib::Request &req, paio::http::Server& server)
{
  return http::Request(req.method, req.body, req.matches, static_cast<void*>(&server));
}

void apply(httplib::Response &response, http::Response &&r)
{
  response.status = r.status;
  response.version = r.version;
  response.body = r.body;
}

struct ServerImpl : public http::Server
{
  ServerImpl(std::string &&addr, const int32_t p) : http::Server(std::forward<std::string>(addr), p),
                                                    svr(new httplib::Server()),
                                                    server_thread(nullptr)
  {
  }

  ServerImpl(const std::string &addr, const int32_t p) : http::Server(addr, p),
                                                         svr(new httplib::Server()),
                                                         server_thread(nullptr)
  {
  }

  ServerImpl(ServerImpl &&s) : http::Server(std::forward<http::Server>(s)),
                               svr(s.svr),
                               server_thread(std::move(s.server_thread))
  {
  }

  virtual ~ServerImpl()
  {
    if (svr->is_running())
    {
      svr->stop();
      server_thread->join();
    }
  }
  std::shared_ptr<httplib::Server> svr;
  std::unique_ptr<std::thread> server_thread;
};

#define _IMPL(x) std::dynamic_pointer_cast<ServerImpl>(x)

http::Server_ptr http::server(std::string &&address, const int32_t port)
{
  auto s = std::shared_ptr<http::Server>(new ServerImpl(std::forward<std::string>(address), port));
  return s;
}

http::Server_ptr http::server(const std::string &address, const int32_t port)
{
  auto s = std::shared_ptr<http::Server>(new ServerImpl(address, port));
  return s;
}

std::function<http::Server_ptr(http::Server_ptr &&)> http::serve(const std::string &endpoint, const std::string &method, Callback cb)
{
  return [&](http::Server_ptr &&server) {
    if (server->failed)
      return server;

    if (method == "GET")
    {
      std::dynamic_pointer_cast<ServerImpl>(server)->svr->Get(endpoint.c_str(), [&server=*server, cb=cb](const httplib::Request &req, httplib::Response &res) {
        apply(res, cb(convert(req, server)));
      });
    }
    else if (method == "PUT")
    {
      std::dynamic_pointer_cast<ServerImpl>(server)->svr->Put(endpoint.c_str(), [=](const httplib::Request &req, httplib::Response &res) {
        apply(res, cb(convert(req)));
      });
    }
    else if (method == "POST")
    {
      std::dynamic_pointer_cast<ServerImpl>(server)->svr->Put(endpoint.c_str(), [=](const httplib::Request &req, httplib::Response &res) {
        apply(res, cb(convert(req)));
      });
    }
    else
    {
      server->failed = true;
      _IMPL(server)->svr = nullptr;
    }
    return server;
  };
}

std::function<http::Server_ptr(http::Server_ptr &&)> http::listen(double timeout)
{
  return [&](http::Server_ptr &&server) {
    int32_t port = server->port;
    auto address = server->address;
    auto svr = _IMPL(server)->svr;
    if (port == 0)
    {
      server->port = svr->bind_to_any_port(address.c_str(), 0);
    }
    _IMPL(server)->server_thread = std::unique_ptr<std::thread>(new std::thread([&] {
      if (port == 0)
      {
        if (svr->listen_after_bind())
        {
        }
      }
      else
      {
        if (svr->listen(address.c_str(), port))
        {
        }
      }
    }));

    auto t = std::chrono::system_clock::now();
    for (;;)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      if (svr->is_running())
        break;

      auto d = std::chrono::system_clock::now() - t;
      int to = (int)(timeout * 1000);

      if (to != 0 && d > std::chrono::milliseconds(to))
      {
        //break;
      }
    }

    //std::cout << "okay" << std::endl;
    server->port = port;
    return server;
  };
}

std::function<http::Server_ptr(http::Server_ptr &&)> http::stop()
{
  return [&](http::Server_ptr &&server) {
    _IMPL(server)->svr->stop();
    _IMPL(server)->server_thread->join();
    return server;
  };
}
