#include "Attribute.hpp"

#include <userver/chaotic/type_bundle_cpp.hpp>

#include "Attribute_parsers.ipp"

namespace post_search {

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs) {
  return lhs.name == rhs.name && lhs.value == rhs.value &&
         lhs.important == rhs.important && lhs.group == rhs.group && true;
}

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(json, to);
}

}  // namespace post_search

