#pragma once

#include <cstdint>
#include <format>
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

  bool operator==(const SingleAsset& other) const = default;
};

using Assets = std::vector<SingleAsset>;

inline std::string toString(const SingleAsset& singleAsset) {
  return std::format("{{assetSymbol={}, freeQuantity={}, usdtValue={}}}", singleAsset.assetSymbol.val_,
                     singleAsset.freeQuantity.val_, singleAsset.usdtValue.val_);
}

}  // namespace ApiGateway