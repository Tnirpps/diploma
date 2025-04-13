#include <userver/components/component.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/congestion_control/component.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>

#include <userver/utils/daemon_run.hpp>

#include "renderer.hpp"

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
                              .Append<userver::components::TestsuiteSupport>()
                              .Append<userver::server::handlers::TestsControl>()
                              .Append<userver::congestion_control::Component>()
                              .AppendComponentList(userver::ugrpc::server::MinimalComponentList())
                              .Append<post_search::Renderer>();
    return userver::utils::DaemonMain(argc, argv, component_list);
}
