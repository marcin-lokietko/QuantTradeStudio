#pragma once

#include "BotBacktester/IBotBacktester.hpp"
#include "BotExecution/IBotExecution.hpp"
#include "IApiGateway.hpp"
#include "MarketService/IMarketService.hpp"

namespace ApiGateway {

class ApiGateway : public IApiGateway {
 public:
  ApiGateway(MarketService::IMarketService& marketService, BotExecution::IBotExecution& botExecution,
             BotBacktester::IBotBacktester& botBacktester);

  AssetValues getOwnedAssets() const override;

  OrderResult makeOrder(const AssetSymbol& selectedBaseAsset, const AssetSymbol& selectedQuoteAsset,
                        const OrderSide& orderSide, const AssetQuantity& baseAssetAmount) const override;

  Orders getOpenOrders() const override;

  AvailableQuoteAssets getAvailableQuoteAssets(const AssetSymbol& baseAsset) const override;

  AssetSymbols getAvailableBaseAssets(const std::optional<AssetSymbol>& quoteAsset = std::nullopt) const override;

  AssetSymbols getQuoteAssetsSuitableForBots() const override;

  StartBotResult startBot(const BotConfig& botConfig) const override;

  StopAllBotsResult stopAllBots() const override;

  BacktestResults testBot(const BotConfig& botConfig, const BacktestConfig& backtestConfig) const override;

 public:
  MarketService::IMarketService& marketService_;
  BotExecution::IBotExecution& botExecution_;
  BotBacktester::IBotBacktester& botBacktester_;
};

}  // namespace ApiGateway