#pragma once

#include <chrono>
#include <string>

namespace ApiGateway {

enum class KlineInterval
{
  OneMinute,
  ThreeMinutes,
  FiveMinutes,
  FifteenMinutes,
  ThirtyMinutes,
  OneHour,
  TwoHours,
  FourHours,
  SixHours,
  EightHours,
  TwelveHours,
  OneDay,
  ThreeDays,
  OneWeek,
  OneMonth
};

inline std::chrono::milliseconds toMilliseconds(const KlineInterval interval) {
  using namespace std::chrono;
  switch (interval) {
    case KlineInterval::OneMinute:
      return minutes(1);
    case KlineInterval::ThreeMinutes:
      return minutes(3);
    case KlineInterval::FiveMinutes:
      return minutes(5);
    case KlineInterval::FifteenMinutes:
      return minutes(15);
    case KlineInterval::ThirtyMinutes:
      return minutes(30);
    case KlineInterval::OneHour:
      return hours(1);
    case KlineInterval::TwoHours:
      return hours(2);
    case KlineInterval::FourHours:
      return hours(4);
    case KlineInterval::SixHours:
      return hours(6);
    case KlineInterval::EightHours:
      return hours(8);
    case KlineInterval::TwelveHours:
      return hours(12);
    case KlineInterval::OneDay:
      return hours(24);
    case KlineInterval::ThreeDays:
      return hours(24 * 3);
    case KlineInterval::OneWeek:
      return hours(24 * 7);
    case KlineInterval::OneMonth:
      return hours(24 * 30);  // Approximate month as 30 days
  }
  throw std::invalid_argument("Invalid KlineInterval");
}
}  // namespace ApiGateway