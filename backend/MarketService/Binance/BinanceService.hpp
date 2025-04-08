#pragma once

#include "MarketService/Binance/Encryption.hpp"
#include "MarketService/IMarketService.hpp"

namespace MarketService::Binance {

class BinanceService : public IMarketService {
 public:
  BinanceService(std::string keysDir) : encryption(std::move(keysDir)){};

  std::string getServerTime() override;

  ApiGateway::Price getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) override;

  AssetPrices getPrices(const std::vector<ApiGateway::TradingPairSymbol>& tradingPairSymbols) const override;

  KlineSequence getKlines(const std::string& symbol, const std::string& interval) override;

  Assets getAssets() const override;

  ApiGateway::OrderResult makeOrder(const ApiGateway::TradingPairSymbol& symbol,
                                    const ApiGateway::AssetQuantity& quantity, const ApiGateway::Price& price) override;

  TradingPairs getTradingPairs(const ApiGateway::AssetSymbol& quoteAsset) const override;

  ApiGateway::Orders getOpenOrders() const override;

 private:
  std::string getAccountUrl() const;
  std::string getOrderUrl(const std::string& queryString) const;

  const std::string keysDir;
  const Encryption encryption;

  const std::string binanceTestnetBaseUrl = "https://testnet.binance.vision";
  const std::string binanceRealBaseUrl = "https://api.binance.com";
};

}  // namespace MarketService::Binance