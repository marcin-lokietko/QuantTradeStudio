#include "BotAlgorithms/DonchianChannelBreakoutStrategy/ExtremumTracker.hpp"

#include "gmock/gmock.h"

namespace BotAlgorithms::DonchianChannelBreakoutStrategy {

using testing::_;
using testing::Return;

TEST(ExtremumValueTrackerTest, WhenHistoryNotLongEnough_ThenComparisonReturnsNullopt) {
  ExtremumTracker<double, std::less<double>> sut{5};

  const double testValue = 5.0;
  EXPECT_FALSE(sut.isValueMoreExtreme(testValue).has_value());

  sut.addValue(2.0);
  EXPECT_FALSE(sut.isValueMoreExtreme(testValue).has_value());
  sut.addValue(4.0);
  EXPECT_FALSE(sut.isValueMoreExtreme(testValue).has_value());
  sut.addValue(6.0);
  EXPECT_FALSE(sut.isValueMoreExtreme(testValue).has_value());
  sut.addValue(8.0);
  EXPECT_FALSE(sut.isValueMoreExtreme(testValue).has_value());
  sut.addValue(10.0);
  EXPECT_TRUE(sut.isValueMoreExtreme(testValue).has_value());
}

TEST(ExtremumTrackerTest, WhenValueGreatest_ThenReturnsTrue) {
  ExtremumTracker<double, std::less<double>> sut{3};

  sut.addValue(1.0);
  sut.addValue(4.99);
  sut.addValue(2.0);

  ASSERT_TRUE(sut.isValueMoreExtreme(4.0).has_value());
  EXPECT_FALSE(sut.isValueMoreExtreme(4.0).value());

  ASSERT_TRUE(sut.isValueMoreExtreme(5.0).has_value());
  EXPECT_TRUE(sut.isValueMoreExtreme(5.0).value());
}

TEST(ExtremumTrackerTest, WhenValueSmallest_ThenReturnsTrue) {
  ExtremumTracker<double, std::greater<double>> sut{3};

  sut.addValue(1.0);
  sut.addValue(4.99);
  sut.addValue(2.0);

  ASSERT_TRUE(sut.isValueMoreExtreme(2.0).has_value());
  EXPECT_FALSE(sut.isValueMoreExtreme(2.0).value());

  ASSERT_TRUE(sut.isValueMoreExtreme(0.5).has_value());
  EXPECT_TRUE(sut.isValueMoreExtreme(0.5).value());
}

TEST(ExtremumTrackerTest, WhenNewValuesAdded_ThenOldestValuesDropped) {
  ExtremumTracker<double, std::less<double>> sut{3};

  sut.addValue(3.0);
  sut.addValue(6.0);
  sut.addValue(9.0);

  ASSERT_TRUE(sut.isValueMoreExtreme(8.0).has_value());
  EXPECT_FALSE(sut.isValueMoreExtreme(8.0).value());

  sut.addValue(7.0);  // This should drop 3.0
  EXPECT_FALSE(sut.isValueMoreExtreme(8.0).value());

  sut.addValue(7.0);  // This should drop 6.0
  EXPECT_FALSE(sut.isValueMoreExtreme(8.0).value());

  sut.addValue(7.0);  // This should drop 9.0
  EXPECT_TRUE(sut.isValueMoreExtreme(8.0).value());
}

}  // namespace BotAlgorithms::DonchianChannelBreakoutStrategy
