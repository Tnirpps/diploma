#pragma once

#include "Promotion.hpp"

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
    k__post_search____Type_Mapping = [](auto selector) {
      return selector()
          .template Type<::post_search:: ::Type, std::string_view>()
          .Case(::post_search:: ::Type::kDiscount, "discount")
          .Case(::post_search:: ::Type::kGift, "gift")
          .Case(::post_search:: ::Type::kInstallment, "installment")
          .Case(::post_search:: ::Type::kCashback, "cashback")
          .Case(::post_search:: ::Type::kSpecial, "special");
    };

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    k__post_search___PropertiesNames = [](auto selector) {
      return selector()
          .template Type<std::string_view>()
          .Case("id")
          .Case("name")
          .Case("description")
          .Case("type")
          .Case("icon")
          .Case("endDate");
    };

template <typename Value>
::post_search:: ::Type Parse(
    Value val, USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Type>) {
  const auto value = val.template As<std::string>();
  const auto result = k__post_search____Type_Mapping.TryFindBySecond(value);
  if (result.has_value()) {
    return *result;
  }
  USERVER_NAMESPACE::chaotic::ThrowForValue(
      fmt::format("Invalid enum value ({}) for type ::post_search::::Type",
                  value),
      val);
}

template <typename Value>
::post_search::Parse(Value value,
                     USERVER_NAMESPACE::formats::parse::To<::post_search::>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  ::post_search::res;

  res.id =
      value["id"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.name =
      value["name"]
          .template As<USERVER_NAMESPACE::chaotic::Primitive<std::string>>();
  res.description =
      value["description"]
          .template As<std::optional<
              USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();
  res.type =
      value["type"]
          .template As<
              USERVER_NAMESPACE::chaotic::Primitive<::post_search:: ::Type>>();
  res.icon = value["icon"]
                 .template As<std::optional<
                     USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();
  res.endDate =
      value["endDate"]
          .template As<std::optional<USERVER_NAMESPACE::chaotic::WithType<
              USERVER_NAMESPACE::chaotic::Primitive<std::string>,
              USERVER_NAMESPACE::utils::datetime::TimePointTz>>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, k__post_search___PropertiesNames);

  return res;
}

}  // namespace post_search

