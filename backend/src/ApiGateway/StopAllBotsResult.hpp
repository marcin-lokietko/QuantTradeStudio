#pragma once

namespace ApiGateway {

enum class StopAllBotsResult
{
  Success = 0,
  Failure
};

inline std::string toString(const StopAllBotsResult& result) {
  switch (result) {
    case StopAllBotsResult::Success:
      return "Success";
    case StopAllBotsResult::Failure:
      return "Failure";
    default:
      return "Unknown";
  }
}
}  // namespace ApiGateway