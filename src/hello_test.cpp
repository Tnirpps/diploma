#include "hello.hpp"

#include <userver/utest/utest.hpp>

UTEST(SayHelloTo, Basic) {
  EXPECT_EQ(search_market_place::SayHelloTo("Developer"), "Hello, Developer!\n");
  EXPECT_EQ(search_market_place::SayHelloTo({}), "Hello, unknown user!\n");
}
