#include "Price.hpp"

#include <userver/chaotic/type_bundle_cpp.hpp>

#include "Price_parsers.ipp"

namespace post_search {

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs) {
  return lhs.value == rhs.value && lhs.currency == rhs.currency &&
         lhs.formatted == rhs.formatted && true;
}

USERVER_NAMESPACE::logging::LogHelper& operator<<(
    USERVER_NAMESPACE::logging::LogHelper& lh,
    const ::post_search:: ::Currency& value) {
  return lh << ToString(value);
}

::Currency Parse(
    USERVER_NAMESPACE::formats::json::Value json,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Currency> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(json, to);
}

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(json, to);
}

::post_search:: ::Currency FromString(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Currency>) {
  const auto result = k__post_search____Currency_Mapping.TryFindBySecond(value);
  if (result.has_value()) {
    return *result;
  }
  throw std::runtime_error(fmt::format(
      "Invalid enum value ({}) for type ::post_search::::Currency", value));
}

::post_search:: ::Currency Parse(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Currency> to) {
  return FromString(value, to);
}

std::string ToString(::post_search:: ::Currency value) {
  const auto result = k__post_search____Currency_Mapping.TryFindByFirst(value);
  if (result.has_value()) {
    return std::string{*result};
  }
  throw std::runtime_error(
      fmt::format("Invalid enum value: {}", static_cast<int>(value)));
}

}  // namespace post_search

fmt::format_context::iterator
fmt::formatter<::post_search:: ::Currency>::format(
    const ::post_search:: ::Currency& value, fmt::format_context& ctx) const {
  return fmt::format_to(ctx.out(), "{}", ToString(value));
}

