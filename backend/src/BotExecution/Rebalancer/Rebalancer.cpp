#include "Rebalancer.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <ranges>
#include <set>
#include <thread>

namespace BotExecution::Rebalancer {

// small transactions are inefficient due to transaction fees
constexpr ApiGateway::SharePercent maxAcceptableShareDeviation{1};

Rebalancer::Rebalancer(Config&& config, const MarketService::IMarketService& marketService,
                       const Wallet::IWallet& wallet, std::unique_ptr<Time::ITime> time)
    : config_(std::move(config)), marketService_(marketService), wallet_(wallet), time_(std::move(time)) {}

void Rebalancer::run(std::stop_token st) {
  SPDLOG_INFO("Rebalancer started execution");
  const std::chrono::seconds executionPeriod(config_.executionPeriod.val_);

  if (!st.stop_requested() && config_.isExecutedImmediately.val_) {
    rebalance();
  }
  while (!st.stop_requested()) {
    time_->sleepFor(st, executionPeriod);
    if (!st.stop_requested()) {
      rebalance();
    }
  }
  SPDLOG_INFO("Rebalancer stopped execution");
}

void Rebalancer::rebalance() {
  SPDLOG_INFO("Rebalancer executes now");

  cancelOpenOrders();

  const auto relevantOwnedAssetValues = getRelevantOwnedAssetValues();
  const auto totalValueOfRelevantOwnedAssets = calcTotalValueOfRelevantOwnedAssets(relevantOwnedAssetValues);

  if (!totalValueOfRelevantOwnedAssets.has_value()) {
    SPDLOG_ERROR("Could not calculate total value of relevant owned assets");
    return;
  }

  auto actualAssetShares = getActualAssetShares(relevantOwnedAssetValues, totalValueOfRelevantOwnedAssets.value());
  if (actualAssetShares.size() != config_.baseAssetShares.size()) {
    SPDLOG_ERROR("Could not calculate actual asset shares");
    return;
  }
  auto expectedAssetShares = config_.baseAssetShares;
  auto sortByAssetSymbols = [](const auto& lhs, const auto& rhs) { return lhs.assetSymbol < rhs.assetSymbol; };
  std::ranges::sort(actualAssetShares, sortByAssetSymbols);
  std::ranges::sort(expectedAssetShares, sortByAssetSymbols);

  SPDLOG_INFO("expectedAssetShares={}", ::toString(expectedAssetShares));
  SPDLOG_INFO("actualAssetShares={}", ::toString(actualAssetShares));

  AssetSharesFloating sharesToSell;
  AssetSharesFloating sharesToBuy;
  for (size_t i = 0; i < actualAssetShares.size(); ++i) {
    if (actualAssetShares[i].assetSymbol != expectedAssetShares[i].assetSymbol) {
      SPDLOG_ERROR("Expected and actual asset share base asset symbols mismatch");
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
  SPDLOG_INFO("sharesToSell={}", ::toString(sharesToSell));
  SPDLOG_INFO("sharesToBuy={}", ::toString(sharesToBuy));

  for (const auto& singleAssetToSell : sharesToSell) {
    const ApiGateway::TradingPairSymbol symbol{singleAssetToSell.assetSymbol.val_ + config_.quoteAsset.val_};
    const ApiGateway::AssetQuantity quoteQuantity{
        std::to_string(totalValueOfRelevantOwnedAssets.value() * singleAssetToSell.share.val_)};
    marketService_.makeMarketTypeOrderWithQuoteQuantity(symbol, ApiGateway::OrderSide::Sell, quoteQuantity);
  }
  for (const auto& singleAssetToBuy : sharesToBuy) {
    const ApiGateway::TradingPairSymbol symbol{singleAssetToBuy.assetSymbol.val_ + config_.quoteAsset.val_};
    const ApiGateway::AssetQuantity quoteQuantity{
        std::to_string(totalValueOfRelevantOwnedAssets.value() * singleAssetToBuy.share.val_)};
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
      SPDLOG_INFO("Cancelling orders on symbol={}", symbol.val_);
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
    SPDLOG_ERROR("Configuration is invalid. The quote asset is not valid for all the base assets.");
    return {};
  }
  return {relevantOwnedAssetValues.begin(), relevantOwnedAssetValues.end()};
}

std::optional<double> Rebalancer::calcTotalValueOfRelevantOwnedAssets(
    const Wallet::AssetValues& relevantOwnedAssetValues) {
  double totalValueOfRelevantOwnedAssets = 0;
  try {
    for (const auto& singleAssetValue : relevantOwnedAssetValues) {
      totalValueOfRelevantOwnedAssets += std::stod(singleAssetValue.value.value().val_);
    }
  } catch (const std::invalid_argument& e) {
    SPDLOG_ERROR("Converting string to double failed - invalid input: {}", e.what());
    return {};
  } catch (const std::out_of_range& e) {
    SPDLOG_ERROR("Converting string to double failed - out of range: {}", e.what());
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