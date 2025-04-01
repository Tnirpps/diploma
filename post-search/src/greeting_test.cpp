#include <greeting.hpp>

#include <userver/utest/utest.hpp>

using post_search::UserType;

UTEST(SayHelloTo, Basic) {
    EXPECT_EQ(post_search::SayHelloTo("Developer", UserType::kFirstTime), "Hello, Developer!\n");
    EXPECT_EQ(post_search::SayHelloTo({}, UserType::kFirstTime), "Hello, unknown user!\n");

    EXPECT_EQ(post_search::SayHelloTo("Developer", UserType::kKnown), "Hi again, Developer!\n");
    EXPECT_EQ(post_search::SayHelloTo({}, UserType::kKnown), "Hi again, unknown user!\n");
}