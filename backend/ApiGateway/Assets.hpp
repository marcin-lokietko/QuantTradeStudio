#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "AssetQuantity.hpp"
#include "AssetSymbol.hpp"
#include "Utils/StrongType.hpp"
#include "Value.hpp"

namespace ApiGateway {

struct SingleAsset {
  AssetSymbol assetSymbol{};
  AssetQuantity freeQuantity{};
  Value usdtValue{};
};

using Assets = std::vector<SingleAsset>;

}  // namespace ApiGateway