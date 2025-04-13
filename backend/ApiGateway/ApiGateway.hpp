#pragma once

#include "IApiGateway.hpp"
#include "MarketService/IMarketService.hpp"
#include "Wallet/IWallet.hpp"

namespace ApiGateway {

class ApiGateway : public IApiGateway {
 public:
  ApiGateway(MarketService::IMarketService& marketService, Wallet::IWallet& wallet);

  Assets getAssets() const override;

  OrderResult makeOrder(const AssetSymbol& selectedBaseAsset, const AssetSymbol& selectedQuoteAsset,
                        const OrderSide& orderSide, const AssetQuantity& baseAssetAmount) const override;

  Orders getOpenOrders() const override;

  AvailableQuoteAssets getAvailableQuoteAssets(const AssetSymbol& baseAsset) const override;

  AssetSymbols getAvailableBaseAssets(const std::optional<AssetSymbol>& quoteAsset = std::nullopt) const override;

  AssetSymbols getQuoteAssetsSuitableForRebalancing() const override;

  StartBotResult startBot(const BotConfig& botConfig) const override;

 public:
  MarketService::IMarketService& marketService_;
  Wallet::IWallet& wallet_;
};

}  // namespace ApiGateway