#include "GuiService/HttpGuiService/Conversion/Orders.hpp"

#include "gmock/gmock.h"

namespace GuiService::HttpGuiService::Conversion {

TEST(OrdersTest, converts) {
  const ApiGateway::SingleOrder singleOrder{
      ApiGateway::TradingPairSymbol{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"USDT"}},
      ApiGateway::OrderId{12345},
      ApiGateway::Price{"50000.0"},
      ApiGateway::AssetQuantity{"1.0"},
      ApiGateway::AssetQuantity{"0.5"},
      ApiGateway::OrderSide::Buy};

  const ApiGateway::Orders orders{singleOrder};

  const auto actualJson = toJson(orders);
  const nlohmann::json expectedJson = nlohmann::json::array({
      nlohmann::json{{"assetPair", "BTCUSDT"},
                     {"orderId", 12345},
                     {"price", "50000.0"},
                     {"origQuantity", "1.0"},
                     {"executedQuantity", "0.5"},
                     {"orderSide", "Buy"}},
  });

  EXPECT_EQ(expectedJson, actualJson);
}
}  // namespace GuiService::HttpGuiService::Conversion
