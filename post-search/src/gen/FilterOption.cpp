#include "FilterOption.hpp"

#include <userver/chaotic/type_bundle_cpp.hpp>

#include "FilterOption_parsers.ipp"

namespace post_search {

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs) {
  return lhs.id == rhs.id && lhs.value == rhs.value && lhs.count == rhs.count &&
         lhs.selected == rhs.selected && lhs.disabled == rhs.disabled &&
         lhs.color == rhs.color && true;
}

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(json, to);
}

}  // namespace post_search

