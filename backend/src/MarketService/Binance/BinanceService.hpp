#pragma once

#include "Config/ConfigParams.hpp"
#include "Http/IHttp.hpp"
#include "MarketService/Binance/IEncryption.hpp"
#include "MarketService/IHistoricalMarketDataProvider.hpp"
#include "MarketService/IMarketService.hpp"
#include "MarketService/TradingPairSymbolDecoder.hpp"
#include "Utils/Time/ITime.hpp"

namespace MarketService::Binance {

class BinanceService : public IMarketService, public IHistoricalMarketDataProvider {
 public:
  BinanceService(const IEncryption& encryption, const Http::IHttp& http, const ::Time::ITime& time,
                 const Config::BinanceUrlPrefix& binanceUrlPrefix)
      : encryption_(encryption), http_(http), time_(time), binanceUrlPrefix_(binanceUrlPrefix) {
    tradingPairSymbolDecoder_ = std::make_unique<TradingPairSymbolDecoder>(getAllTradingPairs());
  }

  Time getServerTime() override;

  ApiGateway::Price getPrice(const ApiGateway::TradingPairSymbol& tradingPairSymbol) override;

  AssetPrices getPrices(const std::vector<ApiGateway::TradingPairSymbol>& tradingPairSymbols) const override;

  KlineSequence getKlines(const ApiGateway::TradingPairSymbol& symbol, const KlineInterval klineInterval,
                          const std::chrono::system_clock::time_point startTime,
                          const std::chrono::system_clock::time_point endTime) const override;

  ApiGateway::AssetQuantities getOwnedAssetsQuantity() const override;

  ApiGateway::AssetValues getOwnedAssetsQuantityAndValue() const override;

  AssetValues getOwnedAssetValues(const ApiGateway::AssetSymbol& quoteAsset) const override;

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
  MarketService::AssetPricesMap getSymbolToPriceMap(const ApiGateway::AssetSymbol& quoteAsset) const;

  const IEncryption& encryption_;
  const Http::IHttp& http_;
  const ::Time::ITime& time_;
  const Config::BinanceUrlPrefix& binanceUrlPrefix_;
  std::unique_ptr<TradingPairSymbolDecoder> tradingPairSymbolDecoder_{};
};

}  // namespace MarketService::Binance