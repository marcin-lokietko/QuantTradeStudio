#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "AssetQuantity.hpp"
#include "AssetSymbol.hpp"
#include "Utils/StrongType.hpp"
#include "ValueInUsdt.hpp"

namespace ApiGateway {

struct SingleAsset {
  AssetSymbol assetSymbol{};
  AssetQuantity freeQuantity{};
  ValueInUsdt usdtValue{};
};

using Assets = std::vector<SingleAsset>;

}  // namespace ApiGateway