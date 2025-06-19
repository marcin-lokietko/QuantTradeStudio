#include "GuiService/HttpGuiService/Conversion/AssetSymbols.hpp"

#include "gmock/gmock.h"

namespace GuiService::HttpGuiService::Conversion {

TEST(AssetSymbolsTest, converts) {
  const ApiGateway::AssetSymbols assetSymbols{ApiGateway::AssetSymbol{"USDT"}, ApiGateway::AssetSymbol{"BTC"},
                                              ApiGateway::AssetSymbol{"ETH"}};

  const auto actualJson = toJson(assetSymbols);
  const nlohmann::json expectedJson =
      nlohmann::json::array({nlohmann::json{{"assetSymbol", "USDT"}}, nlohmann::json{{"assetSymbol", "BTC"}},
                             nlohmann::json{{"assetSymbol", "ETH"}}});
  EXPECT_EQ(expectedJson, actualJson);
}
}  // namespace GuiService::HttpGuiService::Conversion
