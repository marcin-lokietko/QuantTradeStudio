#pragma once

#include <string>

#include "Utils/StrongType.hpp"

namespace ApiGateway {

enum class OrderSide
{
  Invalid = 0,
  Buy,
  Sell,
};

inline std::string toString(OrderSide side) {
  switch (side) {
    case OrderSide::Invalid:
      return "Invalid";
    case OrderSide::Buy:
      return "Buy";
    case OrderSide::Sell:
      return "Sell";
  }
  return "Invalid";
}

}  // namespace ApiGateway