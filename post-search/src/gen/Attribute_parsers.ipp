#pragma once

#include "Attribute.hpp"

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
          .Case("name")
          .Case("value")
          .Case("important")
          .Case("group");
    };

template <typename Value>
::post_search::Parse(Value value,
                     USERVER_NAMESPACE::formats::parse::To<::post_search::>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  ::post_search::res;

  res.name =
      value["name"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.value =
      value["value"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.important =
      value["important"]
          .template As<
              std::optional<USERVER_NAMESPACE::chaotic::Primitive<bool>>>();
  res.group = value["group"]
                  .template As<std::optional<
                      USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, k__post_search___PropertiesNames);

  return res;
}

}  // namespace post_search

