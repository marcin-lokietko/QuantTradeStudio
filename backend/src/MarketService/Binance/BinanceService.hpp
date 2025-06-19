#pragma once

#include "Config/ConfigParams.hpp"
#include "Http/IHttp.hpp"
#include "MarketService/Binance/IEncryption.hpp"
#include "MarketService/IMarketService.hpp"
#include "Utils/Time/ITime.hpp"

namespace MarketService::Binance {

class BinanceService : public IMarketService {
 public:
  BinanceService(const IEncryption& encryption, const Http::IHttp& http, const Time::ITime& time,
                 const Config::BinanceUrlPrefix& binanceUrlPrefix)
      : encryption_(encryption), http_(http), time_(time), binanceUrlPrefix_(binanceUrlPrefix){};

  std::string getServerTime() override;

  ApiGateway::Price getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) override;

  AssetPrices getPrices(const std::vector<ApiGateway::TradingPairSymbol>& tradingPairSymbols) const override;

  KlineSequence getKlines(const std::string& symbol, const std::string& interval) override;

  Assets getOwnedAssets() const override;

  ApiGateway::OrderResult makeOrder(const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
                                    const ApiGateway::AssetQuantity& quantity, const ApiGateway::Price& price) override;

  ApiGateway::OrderResult makeMarketTypeOrderWithQuoteQuantity(
      const ApiGateway::TradingPairSymbol& symbol, const ApiGateway::OrderSide& orderSide,
      const ApiGateway::AssetQuantity& quoteQuantity) const override;

  TradingPairs getAllTradingPairs() const override;
  TradingPairs getTradingPairsWithQuoteAsset(const ApiGateway::AssetSymbol& quoteAsset) const override;
  TradingPairs getTradingPairsWithBaseAsset(const ApiGateway::AssetSymbol& baseAsset) const override;

  ApiGateway::Orders getOpenOrders() const override;

  ApiGateway::OrderResult cancelAllOrdersOnASymbol(const ApiGateway::TradingPairSymbol& symbol) const override;

 private:
  std::string getAccountUrl() const;
  std::string getOrderUrl(const std::string& queryString) const;

  const IEncryption& encryption_;
  const Http::IHttp& http_;
  const Time::ITime& time_;
  const Config::BinanceUrlPrefix& binanceUrlPrefix_;
};

}  // namespace MarketService::Binance