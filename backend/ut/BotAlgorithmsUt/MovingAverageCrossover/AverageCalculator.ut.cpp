#include "BotAlgorithms/MovingAverageCrossover/AverageCalculator.hpp"

#include "gmock/gmock.h"

namespace BotAlgorithms::MovingAverageCrossover {

using testing::_;
using testing::Return;

TEST(AverageCalculatorTest, WhenHistoryNotLongEnough_ThenReturnsNullopt) {
  AverageCalculator<double> calculator{5};

  EXPECT_FALSE(calculator.getAverage().has_value());

  calculator.addValue(1.0);
  EXPECT_FALSE(calculator.getAverage().has_value());
  calculator.addValue(2.0);
  EXPECT_FALSE(calculator.getAverage().has_value());
  calculator.addValue(3.0);
  EXPECT_FALSE(calculator.getAverage().has_value());
  calculator.addValue(4.0);
  EXPECT_FALSE(calculator.getAverage().has_value());
  calculator.addValue(5.0);
  EXPECT_TRUE(calculator.getAverage().has_value());
}

TEST(AverageCalculatorTest, WhenGetAverageCalled_ThenReturnsArithmeticMean) {
  AverageCalculator<double> calculator{3};

  calculator.addValue(3.0);
  calculator.addValue(6.0);
  calculator.addValue(9.0);

  auto average = calculator.getAverage();
  ASSERT_TRUE(average.has_value());
  EXPECT_DOUBLE_EQ(average.value(), 6.0);
}

TEST(AverageCalculatorTest, WhenNewValuesAdded_ThenOldestValuesDropped) {
  AverageCalculator<double> calculator{3};

  calculator.addValue(3.0);
  calculator.addValue(6.0);
  calculator.addValue(9.0);
  calculator.addValue(12.0);

  auto average = calculator.getAverage();
  ASSERT_TRUE(average.has_value());
  EXPECT_DOUBLE_EQ(average.value(), 9.0);
}

}  // namespace BotAlgorithms::MovingAverageCrossover
