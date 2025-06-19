#include "GuiService/HttpGuiService/Conversion/BotConfig.hpp"

#include "gmock/gmock.h"

namespace GuiService::HttpGuiService::Conversion {

TEST(BotConfigTest, converts) {
  const nlohmann::json inputJson = {
      {"botName", "TestBot"},
      {"executionPeriod", 1000},
      {"isExecutedImmediately", true},
      {"quoteAsset", "USDT"},
      {"baseAssetShares", nlohmann::json::array({nlohmann::json{{"assetSymbol", "BTC"}, {"expectedShare", 40}},
                                                 nlohmann::json{{"assetSymbol", "ETH"}, {"expectedShare", 60}}})}};
  ApiGateway::BotConfig actualBotConfig;
  fromJson(inputJson, actualBotConfig);
  const ApiGateway::BotConfig expectedBotConfig{
      ApiGateway::BotName{"TestBot"}, ApiGateway::ExecutionPeriod{1000}, ApiGateway::IsExecutedImmediately{true},
      ApiGateway::AssetSymbol{"USDT"},
      ApiGateway::AssetShares{
          ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::SharePercent{40}},
          ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::SharePercent{60}}}};
  EXPECT_EQ(expectedBotConfig, actualBotConfig);
}
}  // namespace GuiService::HttpGuiService::Conversion
