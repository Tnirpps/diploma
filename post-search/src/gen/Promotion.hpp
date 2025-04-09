#pragma once

#include "Promotion_fwd.hpp"

#include <fmt/core.h>
#include <optional>
#include <string>
#include <userver/chaotic/io/userver/utils/datetime/time_point_tz.hpp>

#include <userver/chaotic/type_bundle_hpp.hpp>

namespace post_search {

// Информация об акции или промо-предложении
struct {
  enum class Type {
    kDiscount,
    kGift,
    kInstallment,
    kCashback,
    kSpecial,
  };

  static constexpr Type kTypeValues[] = {
      Type::kDiscount, Type::kGift,    Type::kInstallment,
      Type::kCashback, Type::kSpecial,
  };

  // CppStringWithFormat

  std::string id{};
  std::string name{};
  std::optional<std::string> description{};
  ::post_search:: ::Type type{};
  std::optional<std::string> icon{};
  std::optional<USERVER_NAMESPACE::utils::datetime::TimePointTz> endDate{};
};

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs);

USERVER_NAMESPACE::logging::LogHelper& operator<<(
    USERVER_NAMESPACE::logging::LogHelper& lh,
    const ::post_search:: ::Type& value);

::Type Parse(USERVER_NAMESPACE::formats::json::Value json,
             USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Type>);

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::>);

::Type FromString(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Type>);

::Type Parse(std::string_view value,
             USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Type>);

std::string ToString(::post_search:: ::Type value);

}  // namespace post_search

template <>
struct fmt::formatter<::post_search:: ::Type> {
  fmt::format_context::iterator format(const ::post_search:: ::Type&,
                                       fmt::format_context&) const;

  constexpr fmt::format_parse_context::iterator parse(
      fmt::format_parse_context& ctx) {
    return ctx.begin();
  }
};

