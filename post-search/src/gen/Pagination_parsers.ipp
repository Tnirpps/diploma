#pragma once

#include "Pagination.hpp"

#include <userver/chaotic/object.hpp>
#include <userver/chaotic/primitive.hpp>
#include <userver/chaotic/validators.hpp>
#include <userver/chaotic/with_type.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace post_search {

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    k__post_search___PropertiesNames = [](auto selector) {
      return selector()
          .template Type<std::string_view>()
          .Case("currentPage")
          .Case("totalPages")
          .Case("totalItems")
          .Case("perPage")
          .Case("hasNextPage")
          .Case("hasPreviousPage");
    };

template <typename Value>
::post_search::Parse(Value value,
                     USERVER_NAMESPACE::formats::parse::To<::post_search::>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  ::post_search::res;

  res.currentPage =
      value["currentPage"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<int>>();
  res.totalPages =
      value["totalPages"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<int>>();
  res.totalItems =
      value["totalItems"]
          .template As<
              std::optional<USERVER_NAMESPACE::chaotic::Primitive<int>>>();
  res.perPage = value["perPage"]
                    .template As<USERVER_NAMESPACE::chaotic::Primitive<int>>();
  res.hasNextPage =
      value["hasNextPage"]
          .template As<
              std::optional<USERVER_NAMESPACE::chaotic::Primitive<bool>>>();
  res.hasPreviousPage =
      value["hasPreviousPage"]
          .template As<
              std::optional<USERVER_NAMESPACE::chaotic::Primitive<bool>>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, k__post_search___PropertiesNames);

  return res;
}

}  // namespace post_search

