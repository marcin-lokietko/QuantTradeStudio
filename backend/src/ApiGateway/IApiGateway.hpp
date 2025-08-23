#pragma once

#include "AssetSymbol.hpp"
#include "AssetSymbols.hpp"
#include "AssetValues.hpp"
#include "AvailableQuoteAssets.hpp"
#include "BacktestConfig.hpp"
#include "BacktestResults.hpp"
#include "BotConfig.hpp"
#include "OrderResult.hpp"
#include "Orders.hpp"
#include "StartBotResult.hpp"
#include "StopAllBotsResult.hpp"

namespace ApiGateway {

class IApiGateway {
 public:
  virtual ~IApiGateway() = default;

  virtual AssetValues getOwnedAssets() const = 0;

  virtual OrderResult makeOrder(const AssetSymbol& selectedBaseAsset, const AssetSymbol& selectedQuoteAsset,
                                const OrderSide& orderSide, const AssetQuantity& baseAssetAmount) const = 0;

  virtual Orders getOpenOrders() const = 0;

  virtual AvailableQuoteAssets getAvailableQuoteAssets(const AssetSymbol& baseAsset) const = 0;

  virtual AssetSymbols getAvailableBaseAssets(const std::optional<AssetSymbol>& quoteAsset = std::nullopt) const = 0;

  virtual AssetSymbols getQuoteAssetsSuitableForRebalancing() const = 0;

  virtual StartBotResult startBot(const BotConfig& botConfig) const = 0;

  virtual StopAllBotsResult stopAllBots() const = 0;

  virtual BacktestResults testBot(const BotConfig& botConfig, const BacktestConfig& backtestConfig) const = 0;
};

}  // namespace ApiGateway