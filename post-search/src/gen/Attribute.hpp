#pragma once

#include "Attribute_fwd.hpp"

#include <optional>
#include <string>

#include <userver/chaotic/type_bundle_hpp.hpp>

namespace post_search {

// Атрибут товара
struct {
  std::string name{};
  std::string value{};
  std::optional<bool> important{};
  std::optional<std::string> group{};
};

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs);

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::>);

}  // namespace post_search

