#include <greeting.hpp>

#include <userver/utest/utest.hpp>

using category::UserType;

UTEST(SayHelloTo, Basic) {
    EXPECT_EQ(category::SayHelloTo("Developer", UserType::kFirstTime), "Hello, Developer!\n");
    EXPECT_EQ(category::SayHelloTo({}, UserType::kFirstTime), "Hello, unknown user!\n");

    EXPECT_EQ(category::SayHelloTo("Developer", UserType::kKnown), "Hi again, Developer!\n");
    EXPECT_EQ(category::SayHelloTo({}, UserType::kKnown), "Hi again, unknown user!\n");
}