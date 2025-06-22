#pragma once

#include <cstdint>
#include <format>
#include <string>

#include "AssetQuantity.hpp"
#include "AssetSymbol.hpp"
#include "Utils/StrongType.hpp"
#include "Value.hpp"

namespace ApiGateway {

struct SingleAssetValue {
  AssetSymbol assetSymbol{};
  AssetQuantity freeQuantity{};
  Value usdtValue{};

  // TODO - should include an AssetSymbol in which the value is expressed

  bool operator==(const SingleAssetValue& other) const = default;
};

inline std::string toString(const SingleAssetValue& singleAsset) {
  return std::format("{{assetSymbol={}, freeQuantity={}, usdtValue={}}}", singleAsset.assetSymbol.val_,
                     singleAsset.freeQuantity.val_, singleAsset.usdtValue.val_);
}

}  // namespace ApiGateway