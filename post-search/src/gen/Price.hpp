#pragma once

#include "Price_fwd.hpp"

#include <fmt/core.h>
#include <optional>
#include <string>

#include <userver/chaotic/type_bundle_hpp.hpp>

namespace post_search {

// Информация о цене
struct {
  enum class Currency {
    kUsd,
    kEur,
    kRub,
    kGbp,
    kCny,
    kJpy,
  };

  static constexpr Currency kCurrencyValues[] = {
      Currency::kUsd, Currency::kEur, Currency::kRub,
      Currency::kGbp, Currency::kCny, Currency::kJpy,
  };

  double value{};
  ::post_search:: ::Currency currency{};
  std::optional<std::string> formatted{};
};

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs);

USERVER_NAMESPACE::logging::LogHelper& operator<<(
    USERVER_NAMESPACE::logging::LogHelper& lh,
    const ::post_search:: ::Currency& value);

::Currency Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Currency>);

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::>);

::Currency FromString(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Currency>);

::Currency Parse(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Currency>);

std::string ToString(::post_search:: ::Currency value);

}  // namespace post_search

template <>
struct fmt::formatter<::post_search:: ::Currency> {
  fmt::format_context::iterator format(const ::post_search:: ::Currency&,
                                       fmt::format_context&) const;

  constexpr fmt::format_parse_context::iterator parse(
      fmt::format_parse_context& ctx) {
    return ctx.begin();
  }
};

