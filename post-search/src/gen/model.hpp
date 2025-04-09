#pragma once

#include "model_fwd.hpp"

#include <cstdint>
#include <optional>
#include <string>

#include <userver/chaotic/type_bundle_hpp.hpp>

namespace post_search {

struct {
  std::string data{};
  int number{};
  bool is_valid{};
};

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs);

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::>);

}  // namespace post_search

