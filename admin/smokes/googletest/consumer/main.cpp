// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Project: adecc Scholar

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdio>

namespace {

bool bAssertionPassed = false;
bool bMockPassed = false;

int Add(int const iLeft, int const iRight) {
   return iLeft + iRight;
}

TEST(GoogleTestSmoke, AssertionWorks) {
   EXPECT_EQ(Add(20, 22), 42);
   bAssertionPassed = !testing::Test::HasFailure();
}

TEST(GoogleMockSmoke, MockWorks) {
   testing::MockFunction<int(int)> aMock;
   EXPECT_CALL(aMock, Call(21)).WillOnce(testing::Return(42));
   EXPECT_EQ(aMock.Call(21), 42);
   bMockPassed = !testing::Test::HasFailure();
}

} // namespace


int main(int iArgc, char** pszArgv) {
   testing::InitGoogleTest(&iArgc, pszArgv);
   int const iResult = RUN_ALL_TESTS();

   std::printf("SMOKE|CHECK|googletest|%s|GoogleTest assertion execution\n",
               bAssertionPassed ? "PASS" : "FAIL");
   std::printf("SMOKE|CHECK|googlemock|%s|GoogleMock expectation execution\n",
               bMockPassed ? "PASS" : "FAIL");

   bool const bPassed = iResult == 0 && bAssertionPassed && bMockPassed;
   std::printf("SMOKE|RESULT|%s|GoogleTest and GoogleMock C++ consumer\n",
               bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : iResult != 0 ? iResult : 1;
}
