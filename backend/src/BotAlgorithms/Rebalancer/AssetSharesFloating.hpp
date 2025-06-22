#pragma once

#include <format>
#include <string>
#include <vector>

#include "ApiGateway/AssetSymbol.hpp"
#include "Utils/StrongType.hpp"

namespace BotAlgorithms::Rebalancer {

DEFINE_STRONG_TYPE(SharePercentFloating, double);

struct SingleAssetShareFloating {
  ApiGateway::AssetSymbol assetSymbol{};
  SharePercentFloating share{};
};

using AssetSharesFloating = std::vector<SingleAssetShareFloating>;

inline std::string toString(const SingleAssetShareFloating& singleAssetShareFloating) {
  return std::format("{{assetSymbol={}, share={}}}", singleAssetShareFloating.assetSymbol.val_,
                     singleAssetShareFloating.share.val_);
}

}  // namespace BotAlgorithms::Rebalancer
