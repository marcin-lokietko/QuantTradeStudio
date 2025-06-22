#include "BotAlgorithms/ConfigExtractor.hpp"

#include "gmock/gmock.h"

namespace BotAlgorithms {

using testing::_;
using testing::Return;

TEST(ConfigExtractorTest, rebalancerConfigIsExtracted) {
  const ApiGateway::BotConfig inputConfig{
      ApiGateway::BotName{"Rebalancer"}, ApiGateway::ExecutionPeriod{1000}, ApiGateway::IsExecutedImmediately{true},
      ApiGateway::AssetSymbol{"USD"},
      ApiGateway::AssetShares{
          {ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{"BTC"}, ApiGateway::SharePercent{50}}},
          {ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{"ETH"}, ApiGateway::SharePercent{50}}}}};

  const auto outputConfig = ConfigExtractor().getConfig(inputConfig);
  ASSERT_TRUE(std::holds_alternative<Rebalancer::Config>(outputConfig));
  const auto& outputRebalancerConfig = std::get<Rebalancer::Config>(outputConfig);
  EXPECT_EQ(outputRebalancerConfig.executionPeriod, inputConfig.executionPeriod);
  EXPECT_EQ(outputRebalancerConfig.isExecutedImmediately, inputConfig.isExecutedImmediately);
  EXPECT_EQ(outputRebalancerConfig.quoteAsset, inputConfig.quoteAsset);
  ASSERT_TRUE(inputConfig.baseAssetShares);
  EXPECT_EQ(outputRebalancerConfig.baseAssetShares, inputConfig.baseAssetShares.value());
}
}  // namespace BotAlgorithms
