#include <iostream>

#include <paio/obj/object.h>
#include <paio/obj/object_dictionary.h>
#include <paio/obj/object_server.h>

paio::ObjectServer paio::object_server()
{
    return ObjectServer();
}

static const paio::OnJsonRead forwarder = [](auto&& oc) { return std::move(oc); };

std::function<paio::ObjectServer(paio::ObjectServer &&)> paio::startUnlock(const std::string &address, const int32_t port)
{
    return [address, port](paio::ObjectServer &&server) {
        server.does(
            [address, port](auto _) { return paio::http::server(address, port); },
            http::serve(R"(/topic/(\w+))", "GET", [od=server.od, js=server.jsonizers, ojr=server.onJsonReads](http::Request &&r) {
                const auto topicName = r.matches[1].str();
                return paio::getContainer<http::Response>(od, topicName, [js, topicName, ojr](const paio::ObjectContainer &oc) {
                    if (paio::isNull(oc)) return http::Response(202);
                    auto jer = js->get(oc.typeName);
                    if (!jer) return http::Response(203); 
                    paio::OnJsonRead onJsonRead = paio::getWithDefault(ojr, topicName, forwarder);
                    return http::Response(200, json::stringify(onJsonRead(jer.value()(oc))));                    
                });
            }),
            http::serve(R"(/topic/(\w+))", "PUT", [od=server.od, cs=server.containizers, ojw=server.onJsonWrites](http::Request &&r) {
                auto topicName = r.matches[1].str();
                return paio::getContainer<http::Response>(od, topicName, [r, od, cs, topicName, ojw](const paio::ObjectContainer &oc) {
                    if (paio::isNull(oc)) return http::Response(202);
                    auto cer = cs->get(oc.typeName);
                    if (!cer) return http::Response(203);
                    paio::OnJsonWrite cb = paio::getWithDefault(ojw, topicName, forwarder);
                    std::cout << "moving....:"  << r.body << std::endl;
                    paio::put(od, topicName, cer.value()(cb(paio::json::parse(r.body))));
                    return http::Response(200);
                });
            })
        );

        return (server >>= http::listen(1.0));
    };
}
