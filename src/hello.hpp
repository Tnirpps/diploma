#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace search_market_place {

std::string SayHelloTo(std::string_view name);

void AppendHello(userver::components::ComponentList &component_list);

} // namespace search_market_place
