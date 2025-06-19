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
  AssetSymbol assetSymbol{};
  SharePercent sharePercent{};

  bool operator==(const SingleAssetShare& other) const = default;
};

using AssetShares = std::vector<SingleAssetShare>;

struct BotConfig {
  BotName botName{};
  std::optional<ExecutionPeriod> executionPeriod{};
  std::optional<IsExecutedImmediately> isExecutedImmediately{};
  std::optional<AssetSymbol> quoteAsset{};
  std::optional<AssetShares> baseAssetShares{};

  bool operator==(const BotConfig& other) const = default;
};

inline std::string toString(const AssetShares& assetShares) {
  std::string ret;
  for (const auto& singleAssetShare : assetShares) {
    ret += "{";
    ret += std::string("assetSymbol=") + singleAssetShare.assetSymbol.val_;
    ret += std::string(", sharePercent=") + std::to_string(singleAssetShare.sharePercent.val_);
    ret += "},";
  }
  if (ret.size() > 0) {
    ret.pop_back();
  }
  return ret;
}
}  // namespace ApiGateway
