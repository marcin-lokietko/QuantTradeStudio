#include "Orders.hpp"

namespace MarketService::Binance::Conversion {
namespace {
ApiGateway::OrderSide toOrderSide(std::string orderSideString) {
  if (orderSideString == "BUY") {
    return ApiGateway::OrderSide::Buy;
  }
  if (orderSideString == "SELL") {
    return ApiGateway::OrderSide::Sell;
  }
  return ApiGateway::OrderSide::Invalid;
}
}  // namespace

void fromJson(const nlohmann::json& j, ApiGateway::Orders& orders,
              const TradingPairSymbolDecoder& tradingPairSymbolDecoder) {
  orders.clear();
  orders.reserve(j.size());

  for (const auto& singleOrder : j) {
    orders.emplace_back(tradingPairSymbolDecoder.decode(singleOrder.at("symbol").get<std::string>()),
                        ApiGateway::OrderId{singleOrder.at("orderId").get<int64_t>()},
                        ApiGateway::Price{singleOrder.at("price").get<std::string>()},
                        ApiGateway::AssetQuantity{singleOrder.at("origQty").get<std::string>()},
                        ApiGateway::AssetQuantity{singleOrder.at("executedQty").get<std::string>()},
                        toOrderSide(singleOrder.at("side").get<std::string>()));
  }
}

}  // namespace MarketService::Binance::Conversion
