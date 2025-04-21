#pragma once

#include <string>
#include <vector>

#include "ApiGateway/AssetSymbol.hpp"
#include "Utils/StrongType.hpp"

namespace BotExecution::Rebalancer {

DEFINE_STRONG_TYPE(SharePercentFloating, double);

struct SingleAssetShareFloating {
  ApiGateway::AssetSymbol assetSymbol{};
  SharePercentFloating share{};
};

using AssetSharesFloating = std::vector<SingleAssetShareFloating>;

inline std::string toString(const AssetSharesFloating& assetShares) {
  std::string ret;
  for (const auto& singleAssetShare : assetShares) {
    ret += "{";
    ret += std::string("assetSymbol=") + singleAssetShare.assetSymbol.val_;
    ret += std::string(", share=") + std::to_string(singleAssetShare.share.val_);
    ret += "},";
  }
  if (ret.size() > 0) {
    ret.pop_back();
  }
  return ret;
}

}  // namespace BotExecution::Rebalancer
