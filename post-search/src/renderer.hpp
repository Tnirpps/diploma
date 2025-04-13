#pragma once

#include <userver/components/component_list.hpp>
#include <userver/components/component.hpp>
#include <userver/ugrpc/server/server.hpp>

#include <post_search.usrv.pb.hpp>

namespace post_search {

class Renderer final : public post_search::ProtoConverter::Service {
public:
    static constexpr std::string_view kName = "post-search-grpc";

    Renderer(const userver::components::ComponentConfig& config,
             const userver::components::ComponentContext& context);

    void RegisterIn(userver::components::ComponentList& component_list);

    grpc::Status ConvertToJson(
        grpc::ServerContext* context,
        const InputMessage* request,
        JsonResponse* response) override;

private:
    userver::ugrpc::server::ServerComponent& server_;
};

}  // namespace post_search
