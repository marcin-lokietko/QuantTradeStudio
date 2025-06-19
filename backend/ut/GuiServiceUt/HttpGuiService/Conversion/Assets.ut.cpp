#include "GuiService/HttpGuiService/Conversion/Assets.hpp"

#include "gmock/gmock.h"

namespace GuiService::HttpGuiService::Conversion {

TEST(AssetsTest, converts) {
  const ApiGateway::Assets assets = {
      ApiGateway::SingleAsset{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetQuantity{"1.0"},
                              ApiGateway::Value{"90000.0"}},
      ApiGateway::SingleAsset{ApiGateway::AssetSymbol{"USDT"}, ApiGateway::AssetQuantity{"1234.0"},
                              ApiGateway::Value{"1234.0"}}};
  const auto actualJson = toJson(assets);

  const nlohmann::json expectedJson =
      nlohmann::json::array({{{"assetSymbol", "BTC"}, {"freeQuantity", "1.0"}, {"usdtValue", "90000.0"}},
                             {{"assetSymbol", "USDT"}, {"freeQuantity", "1234.0"}, {"usdtValue", "1234.0"}}});
  EXPECT_EQ(expectedJson, actualJson);
}
}  // namespace GuiService::HttpGuiService::Conversion
