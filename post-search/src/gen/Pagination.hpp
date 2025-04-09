#pragma once

#include "Pagination_fwd.hpp"

#include <cstdint>
#include <optional>

#include <userver/chaotic/type_bundle_hpp.hpp>

namespace post_search {

// Информация о пагинации
struct {
  int currentPage{};
  int totalPages{};
  std::optional<int> totalItems{};
  int perPage{};
  std::optional<bool> hasNextPage{};
  std::optional<bool> hasPreviousPage{};
};

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs);

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::>);

}  // namespace post_search

