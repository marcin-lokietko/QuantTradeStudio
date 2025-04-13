#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "AssetSymbol.hpp"
#include "Utils/StrongType.hpp"

namespace ApiGateway {

DEFINE_STRONG_TYPE(BotName, std::string);
DEFINE_STRONG_TYPE(ExecutionPeriod, uint64_t);
DEFINE_STRONG_TYPE(SharePercent, uint64_t);
DEFINE_STRONG_TYPE(IsExecutedImmediately, bool);

struct SingleAssetShare {
  AssetSymbol quoteAsset{};
  SharePercent sharePercent{};
};

using AssetShares = std::vector<SingleAssetShare>;

struct BotConfig {
  BotName botName{};
  std::optional<ExecutionPeriod> executionPeriod{};
  std::optional<IsExecutedImmediately> isExecutedImmediately{};
  std::optional<AssetSymbol> quoteAsset{};
  std::optional<AssetShares> baseAssetShares{};
};

}  // namespace ApiGateway

// baseAssetsConfig: this.selectedBaseAssetsConfig,
