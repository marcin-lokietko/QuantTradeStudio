#pragma once

#include <vector>

#include "AssetQuantity.hpp"
#include "OrderId.hpp"
#include "OrderSide.hpp"
#include "Price.hpp"
#include "TradingPairSymbol.hpp"
#include "Utils/StrongType.hpp"

namespace ApiGateway {

struct SingleOrder {
  TradingPairSymbol assetPair{};
  OrderId orderId{};
  Price price{};
  AssetQuantity origQuantity{};
  AssetQuantity executedQuantity{};
  OrderSide side{};

  bool operator==(const SingleOrder& other) const = default;
};

using Orders = std::vector<SingleOrder>;

}  // namespace ApiGateway