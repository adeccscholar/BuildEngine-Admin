// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Project: adecc Scholar

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

int Add(int const iLeft, int const iRight) {
   return iLeft + iRight;
}

TEST(GoogleTestSmoke, AssertionWorks) {
   EXPECT_EQ(Add(20, 22), 42);
}

TEST(GoogleMockSmoke, MockWorks) {
   testing::MockFunction<int(int)> aMock;
   EXPECT_CALL(aMock, Call(21)).WillOnce(testing::Return(42));
   EXPECT_EQ(aMock.Call(21), 42);
}

} // namespace

int main(int iArgc, char** pszArgv) {
   testing::InitGoogleTest(&iArgc, pszArgv);
   return RUN_ALL_TESTS();
}
