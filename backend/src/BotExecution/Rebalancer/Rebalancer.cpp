#include <algorithm>
#include <ranges>
#include <set>
#include <thread>

#include "Rebalancer.hpp"
#include "glog/logging.h"

namespace BotExecution::Rebalancer {

// small transactions are inefficient due to transaction fees
constexpr ApiGateway::SharePercent maxAcceptableShareDeviation{1};

Rebalancer::Rebalancer(Config&& config, const MarketService::IMarketService& marketService,
                       const Wallet::IWallet& wallet)
    : config_(std::move(config)), marketService_(marketService), wallet_(wallet) {}

void Rebalancer::run(std::stop_token st) {
  LOG(INFO) << "Rebalancer started execution";
  const std::chrono::seconds executionPeriod(config_.executionPeriod.val_);

  if (!st.stop_requested() && config_.isExecutedImmediately.val_) {
    rebalance();
  }
  while (!st.stop_requested()) {
    std::this_thread::sleep_for(executionPeriod);
    if (!st.stop_requested()) {
      rebalance();
    }
  }
  LOG(INFO) << "Rebalancer stopped execution";
}

void Rebalancer::rebalance() {
  LOG(INFO) << "Rebalancer executes now";

  cancelOpenOrders();

  const auto relevantOwnedAssetValues = getRelevantOwnedAssetValues();
  const auto totalValueOfRelevantOwnedAssets = calcTotalValueOfRelevantOwnedAssets(relevantOwnedAssetValues);

  auto actualAssetShares = getActualAssetShares(relevantOwnedAssetValues, totalValueOfRelevantOwnedAssets);
  if (actualAssetShares.size() != config_.baseAssetShares.size()) {
    LOG(ERROR) << "Could not calculate actual asset shares";
    return;
  }
  auto expectedAssetShares = config_.baseAssetShares;
  auto sortByAssetSymbols = [](const auto& lhs, const auto& rhs) { return lhs.assetSymbol < rhs.assetSymbol; };
  std::ranges::sort(actualAssetShares, sortByAssetSymbols);
  std::ranges::sort(expectedAssetShares, sortByAssetSymbols);

  LOG(INFO) << "expectedAssetShares=" << toString(expectedAssetShares);
  LOG(INFO) << "actualAssetShares=" << toString(actualAssetShares);

  AssetSharesFloating sharesToSell;
  AssetSharesFloating sharesToBuy;
  for (size_t i = 0; i < actualAssetShares.size(); ++i) {
    if (actualAssetShares[i].assetSymbol != expectedAssetShares[i].assetSymbol) {
      LOG(ERROR) << "Expected and actual asset share base asset symbols mismatch";
      return;
    }
    const auto& assetSymbol = actualAssetShares[i].assetSymbol;
    const auto actualShare = actualAssetShares[i].share.val_;
    const auto actualSharePercent = actualShare * 100;
    const auto expectedSharePercent = expectedAssetShares[i].sharePercent.val_;

    if (actualSharePercent > expectedSharePercent + maxAcceptableShareDeviation.val_) {
      const double diff = (actualSharePercent - expectedSharePercent) / 100;
      sharesToSell.emplace_back(assetSymbol, SharePercentFloating{diff});
    }
    if (actualSharePercent + maxAcceptableShareDeviation.val_ < expectedSharePercent) {
      const double diff = (expectedSharePercent - actualSharePercent) / 100;
      sharesToBuy.emplace_back(assetSymbol, SharePercentFloating{diff});
    }
  }
  LOG(INFO) << "sharesToSell=" << toString(sharesToSell);
  LOG(INFO) << "sharesToBuy=" << toString(sharesToBuy);

  for (const auto& singleAssetToSell : sharesToSell) {
    const ApiGateway::TradingPairSymbol symbol{singleAssetToSell.assetSymbol.val_ + config_.quoteAsset.val_};
    const ApiGateway::AssetQuantity quoteQuantity{
        std::to_string(totalValueOfRelevantOwnedAssets * singleAssetToSell.share.val_)};
    marketService_.makeMarketTypeOrderWithQuoteQuantity(symbol, ApiGateway::OrderSide::Sell, quoteQuantity);
  }
  for (const auto& singleAssetToBuy : sharesToBuy) {
    const ApiGateway::TradingPairSymbol symbol{singleAssetToBuy.assetSymbol.val_ + config_.quoteAsset.val_};
    const ApiGateway::AssetQuantity quoteQuantity{
        std::to_string(totalValueOfRelevantOwnedAssets * singleAssetToBuy.share.val_)};
    marketService_.makeMarketTypeOrderWithQuoteQuantity(symbol, ApiGateway::OrderSide::Buy, quoteQuantity);
  }
}

void Rebalancer::cancelOpenOrders() {
  const auto openOrders = marketService_.getOpenOrders() |
                          std::views::transform([](const auto& singleOrder) { return singleOrder.assetPair; });
  std::set<ApiGateway::TradingPairSymbol> openOrdersSymbols{openOrders.begin(), openOrders.end()};

  for (const auto& singleBaseAssetShare : config_.baseAssetShares) {
    const ApiGateway::TradingPairSymbol symbol{singleBaseAssetShare.assetSymbol.val_ + config_.quoteAsset.val_};

    if (openOrdersSymbols.count(symbol)) {
      LOG(INFO) << "Cancelling orders on symbol=" << symbol.val_;
      marketService_.cancelAllOrdersOnASymbol(symbol);
    }
  }
}

Wallet::AssetValues Rebalancer::getRelevantOwnedAssetValues() {
  const auto configuredBaseAssets = config_.baseAssetShares | std::views::transform([](const auto& singleAssetShare) {
                                      return singleAssetShare.assetSymbol;
                                    });

  const auto ownedAssetValues = wallet_.getOwnedAssetValues(config_.quoteAsset);
  auto relevantOwnedAssetValues =
      ownedAssetValues | std::views::filter([&configuredBaseAssets](const auto& singleAssetValue) {
        return std::ranges::find(configuredBaseAssets, singleAssetValue.baseSymbol) != configuredBaseAssets.end();
      });

  if (std::ranges::any_of(relevantOwnedAssetValues, [&config_ = config_](const auto& singleAssetValue) {
        return !singleAssetValue.value.has_value() || singleAssetValue.quoteAsset != config_.quoteAsset;
      })) {
    LOG(ERROR) << "Configuration is invalid. The quote asset is not valid for all the base assets.";
    return {};
  }
  return {relevantOwnedAssetValues.begin(), relevantOwnedAssetValues.end()};
}

double Rebalancer::calcTotalValueOfRelevantOwnedAssets(const Wallet::AssetValues& relevantOwnedAssetValues) {
  double totalValueOfRelevantOwnedAssets = 0;
  try {
    for (const auto& singleAssetValue : relevantOwnedAssetValues) {
      totalValueOfRelevantOwnedAssets += std::stod(singleAssetValue.value.value().val_);
    }
  } catch (const std::invalid_argument& e) {
    LOG(ERROR) << "Converting string to double failed - invalid input: " << e.what();
    return {};
  } catch (const std::out_of_range& e) {
    LOG(ERROR) << "Converting string to double failed - out of range: " << e.what();
    return {};
  }
  return totalValueOfRelevantOwnedAssets;
}

AssetSharesFloating Rebalancer::getActualAssetShares(const Wallet::AssetValues& relevantOwnedAssetValues,
                                                     const double totalValueOfRelevantOwnedAssets) {
  AssetSharesFloating assetShares;
  for (const auto& singleAssetValue : relevantOwnedAssetValues) {
    const double sharePercent = std::stod(singleAssetValue.value.value().val_) / totalValueOfRelevantOwnedAssets;
    assetShares.emplace_back(singleAssetValue.baseSymbol, SharePercentFloating{sharePercent});
  }
  return assetShares;
}

}  // namespace BotExecution::Rebalancer