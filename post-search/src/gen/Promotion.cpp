#include "Promotion.hpp"

#include <userver/chaotic/type_bundle_cpp.hpp>

#include "Promotion_parsers.ipp"

namespace post_search {

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs) {
  return lhs.id == rhs.id && lhs.name == rhs.name &&
         lhs.description == rhs.description && lhs.type == rhs.type &&
         lhs.icon == rhs.icon && lhs.endDate == rhs.endDate && true;
}

USERVER_NAMESPACE::logging::LogHelper& operator<<(
    USERVER_NAMESPACE::logging::LogHelper& lh,
    const ::post_search:: ::Type& value) {
  return lh << ToString(value);
}

::Type Parse(USERVER_NAMESPACE::formats::json::Value json,
             USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Type> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(json, to);
}

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(json, to);
}

::post_search:: ::Type FromString(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Type>) {
  const auto result = k__post_search____Type_Mapping.TryFindBySecond(value);
  if (result.has_value()) {
    return *result;
  }
  throw std::runtime_error(fmt::format(
      "Invalid enum value ({}) for type ::post_search::::Type", value));
}

::post_search:: ::Type Parse(
    std::string_view value,
    USERVER_NAMESPACE::formats::parse::To<::post_search:: ::Type> to) {
  return FromString(value, to);
}

std::string ToString(::post_search:: ::Type value) {
  const auto result = k__post_search____Type_Mapping.TryFindByFirst(value);
  if (result.has_value()) {
    return std::string{*result};
  }
  throw std::runtime_error(
      fmt::format("Invalid enum value: {}", static_cast<int>(value)));
}

}  // namespace post_search

fmt::format_context::iterator fmt::formatter<::post_search:: ::Type>::format(
    const ::post_search:: ::Type& value, fmt::format_context& ctx) const {
  return fmt::format_to(ctx.out(), "{}", ToString(value));
}

