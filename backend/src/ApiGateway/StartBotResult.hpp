#pragma once

namespace ApiGateway {

enum class StartBotResult
{
  Success = 0,
  Failure
};

inline std::string toString(const StartBotResult& result) {
  switch (result) {
    case StartBotResult::Success:
      return "Success";
    case StartBotResult::Failure:
      return "Failure";
    default:
      return "Unknown";
  }
}

}  // namespace ApiGateway