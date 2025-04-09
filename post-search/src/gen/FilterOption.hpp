#pragma once

#include "FilterOption_fwd.hpp"

#include <cstdint>
#include <optional>
#include <string>

#include <userver/chaotic/type_bundle_hpp.hpp>

namespace post_search {

// Вариант значения фильтра
struct {
  std::string id{};
  std::string value{};
  int count{};
  std::optional<bool> selected{};
  std::optional<bool> disabled{};
  std::optional<std::string> color{};
};

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs);

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::>);

}  // namespace post_search

