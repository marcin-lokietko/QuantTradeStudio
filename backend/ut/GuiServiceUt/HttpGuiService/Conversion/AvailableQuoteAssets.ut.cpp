#include "GuiService/HttpGuiService/Conversion/AvailableQuoteAssets.hpp"

#include "gmock/gmock.h"

namespace GuiService::HttpGuiService::Conversion {

TEST(AvailableQuoteAssetsTest, converts) {
  const ApiGateway::AvailableQuoteAssets availableQuoteAssets{
      ApiGateway::SingleAvailableQuoteAsset{ApiGateway::AssetSymbol{"USDT"}, ApiGateway::Price{"1.0"}},
      ApiGateway::SingleAvailableQuoteAsset{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::Price{"50000.0"}},
      ApiGateway::SingleAvailableQuoteAsset{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::Price{"3000.0"}}};

  const auto actualJson = toJson(availableQuoteAssets);
  const nlohmann::json expectedJson =
      nlohmann::json::array({nlohmann::json{{"quoteAsset", "USDT"}, {"baseAssetUnitPrice", "1.0"}},
                             nlohmann::json{{"quoteAsset", "BTC"}, {"baseAssetUnitPrice", "50000.0"}},
                             nlohmann::json{{"quoteAsset", "ETH"}, {"baseAssetUnitPrice", "3000.0"}}});

  EXPECT_EQ(expectedJson, actualJson);
}
}  // namespace GuiService::HttpGuiService::Conversion
