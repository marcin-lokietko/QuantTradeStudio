#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Utils/StrongType.hpp"

namespace Wallet {

DEFINE_STRONG_TYPE(AssetSymbol, std::string);
DEFINE_STRONG_TYPE(AssetQuantity, std::string);
DEFINE_STRONG_TYPE(ValueInUsdt, std::string);

struct SingleAsset {
  AssetSymbol assetSymbol{};
  AssetQuantity freeQuantity{};
  ValueInUsdt usdtValue{};
};

using Assets = std::vector<SingleAsset>;

}  // namespace Wallet