#include "BotExecution/Rebalancer/Rebalancer.hpp"
#include "CommonMocks/MarketServiceMock.hpp"
#include "CommonMocks/TimeMock.hpp"
#include "CommonMocks/WalletMock.hpp"
#include "gmock/gmock.h"

namespace BotExecution::Rebalancer {

using ApiGateway::AssetSymbol;
using ApiGateway::ExecutionPeriod;
using ApiGateway::IsExecutedImmediately;
using ApiGateway::SharePercent;
using ApiGateway::SingleAssetShare;
using testing::Return;

const ApiGateway::AssetShares assetShares{SingleAssetShare{AssetSymbol{"BTC"}, SharePercent{80}},
                                          SingleAssetShare{AssetSymbol{"ETH"}, SharePercent{20}}};

const Config config{.executionPeriod = ExecutionPeriod{60},
                    .isExecutedImmediately = IsExecutedImmediately{false},
                    .quoteAsset = AssetSymbol{"USDT"},
                    .baseAssetShares = assetShares};

class RebalancerTest : public ::testing::Test {
 public:
  Rebalancer getSut(Config config, std::unique_ptr<Time::TimeMock> time) {
    return {std::move(config), marketServiceMock_, walletMock_, std::move(time)};
  }

  MarketService::MarketServiceMock marketServiceMock_;
  Wallet::WalletMock walletMock_;
};

TEST_F(RebalancerTest, WhenConfiguredBaseAssetsNowOwned_ThenBotDoesNotTrade) {
  std::stop_source source;
  std::stop_token token = source.get_token();

  EXPECT_CALL(marketServiceMock_, getOpenOrders()).WillOnce([&source]() {
    // Request stop, so that the bot executes only once
    source.request_stop();
    return ApiGateway::Orders{};
  });
  EXPECT_CALL(walletMock_, getOwnedAssetValues(ApiGateway::AssetSymbol{"USDT"}))
      .WillOnce(Return(Wallet::AssetValues{}));

  auto time = std::make_unique<Time::TimeMock>();
  EXPECT_CALL(*time, sleepFor(std::chrono::seconds(config.executionPeriod.val_))).WillOnce(Return());

  getSut(config, std::move(time)).run(token);
}
}  // namespace BotExecution::Rebalancer
