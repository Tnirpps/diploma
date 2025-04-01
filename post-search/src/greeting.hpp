#pragma once

#include <string>
#include <string_view>

namespace post_search {

enum class UserType { kFirstTime, kKnown };

std::string SayHelloTo(std::string_view name, UserType type);

}  // namespace post_search