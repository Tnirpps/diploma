#pragma once

#include "Price.hpp"

#include <userver/chaotic/exception.hpp>
#include <userver/chaotic/object.hpp>
#include <userver/chaotic/primitive.hpp>
#include <userver/chaotic/validators.hpp>
#include <userver/chaotic/with_type.hpp>
#include <userver/formats/parse/common_containers.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/utils/trivial_map.hpp>

namespace post_search {

static constexpr USERVER_NAMESPACE::utils::TrivialBiMap
    k__post_search____Currency_Mapping = [](auto selector) {
      return selector()
          .template Type<::post_search:: ::Currency, std::string_view>()
          .Case(::post_search:: ::Currency::kUsd, "USD")
          .Case(::post_search:: ::Currency::kEur, "EUR")
          .Case(::post_search:: ::Currency::kRub, "RUB")
          .Case(::post_search:: ::Currency::kGbp, "GBP")
          .Case(::post_search:: ::Currency::kCny, "CNY")
          .Case(::post_search:: ::Currency::kJpy, "JPY");
    };

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    k__post_search___PropertiesNames = [](auto selector) {
      return selector()
          .template Type<std::string_view>()
          .Case("value")
          .Case("currency")
          .Case("formatted");
    };

template <typename Value>
::post_search:: ::Currency Parse(
    Value val,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Currency>) {
  const auto value = val.template As<std::string>();
  const auto result = k__post_search____Currency_Mapping.TryFindBySecond(value);
  if (result.has_value()) {
    return *result;
  }
  USERVER_NAMESPACE::chaotic::ThrowForValue(
      fmt::format("Invalid enum value ({}) for type ::post_search::::Currency",
                  value),
      val);
}

template <typename Value>
::post_search::Parse(Value value,
                     USERVER_NAMESPACE::formats::parse::To<::post_search::>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  ::post_search::res;

  res.value = value["value"]
                  .template As<USERVER_NAMESPACE::chaotic::Primitive<double>>();
  res.currency = value["currency"]
                     .template As<USERVER_NAMESPACE::chaotic::Primitive<
                         ::post_search:: ::Currency>>();
  res.formatted =
      value["formatted"]
          .template As<std::optional<
              USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, k__post_search___PropertiesNames);

  return res;
}

}  // namespace post_search

