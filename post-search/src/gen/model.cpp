#include "model.hpp"

#include <userver/chaotic/type_bundle_cpp.hpp>

#include "model_parsers.ipp"

namespace post_search {

bool operator==(const ::post_search::&lhs, const ::post_search::&rhs) {
  return lhs.data == rhs.data && lhs.number == rhs.number &&
         lhs.is_valid == rhs.is_valid && true;
}

Parse(USERVER_NAMESPACE::formats::json::Value json,
      USERVER_NAMESPACE::formats::parse::To<::post_search::> to) {
  return Parse<USERVER_NAMESPACE::formats::json::Value>(json, to);
}

}  // namespace post_search

