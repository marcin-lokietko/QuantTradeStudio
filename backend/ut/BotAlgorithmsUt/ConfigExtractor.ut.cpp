#include "BotAlgorithms/ConfigExtractor.hpp"

#include "gmock/gmock.h"

namespace BotAlgorithms {

using testing::_;
using testing::Return;

TEST(ConfigExtractorTest, rebalancerConfigIsExtracted) {
  ApiGateway::BotConfig inputConfig{};

  inputConfig.botName = ApiGateway::BotName{"Rebalancer"};
  inputConfig.executionPeriod = ApiGateway::ExecutionPeriod{1000};
  inputConfig.isExecutedImmediately = ApiGateway::IsExecutedImmediately{true};
  inputConfig.quoteAsset = ApiGateway::AssetSymbol{"USD"};
  inputConfig.baseAssetShares = ApiGateway::AssetShares{
      {ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::SharePercent{50}}},
      {ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::SharePercent{50}}}};

  const auto outputConfig = ConfigExtractor().getConfig(inputConfig);
  ASSERT_TRUE(std::holds_alternative<Rebalancer::Config>(outputConfig));
  const auto& outputRebalancerConfig = std::get<Rebalancer::Config>(outputConfig);
  EXPECT_EQ(outputRebalancerConfig.executionPeriod, inputConfig.executionPeriod);
  EXPECT_EQ(outputRebalancerConfig.isExecutedImmediately, inputConfig.isExecutedImmediately);
  EXPECT_EQ(outputRebalancerConfig.quoteAsset, inputConfig.quoteAsset);
  ASSERT_TRUE(inputConfig.baseAssetShares);
  EXPECT_EQ(outputRebalancerConfig.baseAssetShares, inputConfig.baseAssetShares.value());
}

TEST(ConfigExtractorTest, movingAverageCrossoverConfigIsExtracted) {
  ApiGateway::BotConfig inputConfig{};

  inputConfig.botName = ApiGateway::BotName{"MovingAverageCrossover"};
  inputConfig.executionPeriod = ApiGateway::ExecutionPeriod{2000};
  inputConfig.shortTermMovingAverageLength = ApiGateway::AssetHistoryLength{5};
  inputConfig.longTermMovingAverageLength = ApiGateway::AssetHistoryLength{20};
  inputConfig.quoteAsset = ApiGateway::AssetSymbol{"USD"};
  inputConfig.baseAssets = ApiGateway::AssetSymbols{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::AssetSymbol{"ETH"}};

  const auto outputConfig = ConfigExtractor().getConfig(inputConfig);
  ASSERT_TRUE(std::holds_alternative<MovingAverageCrossover::Config>(outputConfig));
  const auto& outputMacConfig = std::get<MovingAverageCrossover::Config>(outputConfig);
  EXPECT_EQ(outputMacConfig.executionPeriod, inputConfig.executionPeriod);
  EXPECT_EQ(outputMacConfig.shortTermMovingAverageLength, inputConfig.shortTermMovingAverageLength);
  EXPECT_EQ(outputMacConfig.longTermMovingAverageLength, inputConfig.longTermMovingAverageLength);
  EXPECT_EQ(outputMacConfig.quoteAsset, inputConfig.quoteAsset);
  ASSERT_TRUE(inputConfig.baseAssets);
  EXPECT_EQ(outputMacConfig.baseAssets, inputConfig.baseAssets.value());
}

}  // namespace BotAlgorithms
