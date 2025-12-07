#pragma once

#include <chrono>
#include <string>

namespace ApiGateway {

enum class KlineInterval
{
  Invalid,
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

inline std::string toReadableString(const KlineInterval interval) {
  switch (interval) {
    case KlineInterval::Invalid:
      return "Invalid";
    case KlineInterval::OneMinute:
      return "OneMinute";
    case KlineInterval::ThreeMinutes:
      return "ThreeMinutes";
    case KlineInterval::FiveMinutes:
      return "FiveMinutes";
    case KlineInterval::FifteenMinutes:
      return "FifteenMinutes";
    case KlineInterval::ThirtyMinutes:
      return "ThirtyMinutes";
    case KlineInterval::OneHour:
      return "OneHour";
    case KlineInterval::TwoHours:
      return "TwoHours";
    case KlineInterval::FourHours:
      return "FourHours";
    case KlineInterval::SixHours:
      return "SixHours";
    case KlineInterval::EightHours:
      return "EightHours";
    case KlineInterval::TwelveHours:
      return "TwelveHours";
    case KlineInterval::OneDay:
      return "OneDay";
    case KlineInterval::ThreeDays:
      return "ThreeDays";
    case KlineInterval::OneWeek:
      return "OneWeek";
    case KlineInterval::OneMonth:
      return "OneMonth";
  }
  return "Invalid";
}

inline std::chrono::milliseconds toMilliseconds(const KlineInterval interval) {
  using namespace std::chrono;
  switch (interval) {
    case KlineInterval::Invalid:
      throw std::invalid_argument("Invalid KlineInterval");
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