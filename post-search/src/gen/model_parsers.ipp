#pragma once

#include "model.hpp"

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
          .Case("data")
          .Case("number")
          .Case("is_valid");
    };

template <typename Value>
::post_search::Parse(Value value,
                     USERVER_NAMESPACE::formats::parse::To<::post_search::>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  ::post_search::res;

  res.data =
      value["data"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.number =
      value["number"].template As<USERVER_NAMESPACE::chaotic::Primitive<int>>();
  res.is_valid =
      value["is_valid"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<bool>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, k__post_search___PropertiesNames);

  return res;
}

}  // namespace post_search

