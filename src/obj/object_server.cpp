#include <iostream>

#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/obj/object_server.h>

paio::ObjectServer paio::object_server()
{
    return ObjectServer();
}

paio::ObjectServer paio::startUnlock(paio::ObjectServer &&server, const std::string &address, const int32_t port)
{
    auto srv = paio::http::server(address, port);
    std::function<void(const std::string &, paio::ObjectContainer &)> f = [&srv, js = server.js, cs = server.cs, od = server.od](const std::string &topic,
                                                                                                                                 paio::ObjectContainer &container) {
        auto serv = http::serve(topic, "GET", [&container, js, cs, od](http::Request &&r) {
            if (js->count(container.typeName) != 0)
            {
                return http::Response(200, json::stringify((*js)[container.typeName](container)));
            }
            return http::Response(202, container.typeName);
        },
                                std::move(srv));
        serv = http::serve(topic, "PUT", [&container, &topic, js, cs, od](http::Request &&r) {
            if (cs->count(container.typeName) != 0)
            {
                paio::Containizer c = (*cs)[container.typeName];
                auto cc = paio::json::parse(r.body);
                paio::put(od, topic, c(cc));
                return http::Response(200);
            }
            return http::Response(202, container.typeName);
        },
                           std::move(serv));
    };
    paio::forEach(server.od, f);
    server.srv = http::listen(1.0, std::move(srv));
    return std::move(server);
}
