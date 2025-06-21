#pragma once

#include <format>
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

inline std::string toString(const SingleOrder& singleOrder) {
  return std::format("{{assetPair={}, orderId={}, price={}, origQuantity={}, executedQuantity={}, side={}}}",
                     singleOrder.assetPair.val_, singleOrder.orderId.val_, singleOrder.price.val_,
                     singleOrder.origQuantity.val_, singleOrder.executedQuantity.val_, toString(singleOrder.side));
}

}  // namespace ApiGateway