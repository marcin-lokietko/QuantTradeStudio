#include "gmock/gmock.h"

// TEST* macros implicitly register their tests with Google Test. So, unlike with many other C++ testing frameworks,
// you don't have to re-list all your defined tests in order to run them.

int main(int argc, char **argv) {
  // The ::testing::InitGoogleTest() function parses the command line for Google Test flags, and removes all recognized
  // flags. This allows the user to control a test program's behavior via various flags. You must call this function
  // before calling RUN_ALL_TESTS(), or the flags won't be properly initialized.
  // ::testing::InitGoogleTest(&argc, argv);
  // if google mock also used - use the line above, which setups both gtest and gmock
  ::testing::InitGoogleMock(&argc, argv);

  // RUN_ALL_TESTS() runs all tests in your link unit -- they can be from different test cases, or even different source
  // files.

  // You must not ignore the return value of RUN_ALL_TESTS(), or gcc will give you a compiler error.
  // The rationale for this design is that the automated testing service determines whether
  // a test has passed based on its exit code, not on its stdout/stderr output; thus your main() function
  // must return the value of RUN_ALL_TESTS().

  // Also, you should call RUN_ALL_TESTS() only once. Calling it more than once conflicts with some advanced
  // Google Test features (e.g. thread-safe death tests) and thus is not supported.
  return RUN_ALL_TESTS();
}

// Google Test is designed to be thread-safe. The implementation is thread-safe on systems where the pthreads library is
// available. It is currently unsafe to use Google Test assertions from two threads concurrently on other systems (e.g.
// Windows). In most tests this is not an issue as usually the assertions are done in the main thread.