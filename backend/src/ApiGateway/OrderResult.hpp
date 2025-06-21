#pragma once

namespace ApiGateway {

enum class OrderResult
{
  Success = 0,
  Failure
};

inline std::string toString(OrderResult orderResult) {
  switch (orderResult) {
    case OrderResult::Success:
      return "Success";
    case OrderResult::Failure:
      return "Failure";
  }
  return "Invalid";
}
}  // namespace ApiGateway