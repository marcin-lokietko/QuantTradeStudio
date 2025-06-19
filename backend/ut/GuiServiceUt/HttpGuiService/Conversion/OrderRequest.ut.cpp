#include "GuiService/HttpGuiService/Conversion/OrderRequest.hpp"

#include "gmock/gmock.h"

namespace GuiService::HttpGuiService::Conversion {

TEST(OrderRequestTest, converts) {
  const nlohmann::json inputJson = {
      {"selectedBaseAsset", "USDT"}, {"selectedQuoteAsset", "BTC"}, {"orderSide", "Buy"}, {"baseAssetAmount", "100.0"}};

  const GuiService::OrderRequest expectedOrderRequest{.selectedBaseAsset = ApiGateway::AssetSymbol{"USDT"},
                                                      .selectedQuoteAsset = ApiGateway::AssetSymbol{"BTC"},
                                                      .orderSide = ApiGateway::OrderSide::Buy,
                                                      .baseAssetAmount = ApiGateway::AssetQuantity{"100.0"}};

  GuiService::OrderRequest actualOrderRequest;
  fromJson(inputJson, actualOrderRequest);
  EXPECT_EQ(expectedOrderRequest, actualOrderRequest);
}
}  // namespace GuiService::HttpGuiService::Conversion
