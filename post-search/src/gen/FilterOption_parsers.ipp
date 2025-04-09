#pragma once

#include "FilterOption.hpp"

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
          .Case("id")
          .Case("value")
          .Case("count")
          .Case("selected")
          .Case("disabled")
          .Case("color");
    };

template <typename Value>
::post_search::Parse(Value value,
                     USERVER_NAMESPACE::formats::parse::To<::post_search::>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  ::post_search::res;

  res.id =
      value["id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.value =
      value["value"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.count =
      value["count"].template As<USERVER_NAMESPACE::chaotic::Primitive<int>>();
  res.selected =
      value["selected"]
          .template As<
              std::optional<USERVER_NAMESPACE::chaotic::Primitive<bool>>>();
  res.disabled =
      value["disabled"]
          .template As<
              std::optional<USERVER_NAMESPACE::chaotic::Primitive<bool>>>();
  res.color = value["color"]
                  .template As<std::optional<
                      USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, k__post_search___PropertiesNames);

  return res;
}

}  // namespace post_search

