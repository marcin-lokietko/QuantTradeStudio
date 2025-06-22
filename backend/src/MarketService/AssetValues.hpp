#pragma once

#include <optional>
#include <vector>

#include "ApiGateway/AssetSymbol.hpp"
#include "ApiGateway/Value.hpp"
#include "Utils/ToString.hpp"

namespace MarketService {

struct SingleAssetValue {
  ApiGateway::AssetSymbol baseSymbol{};
  ApiGateway::AssetSymbol quoteAsset{};
  std::optional<ApiGateway::Value> value{};
};

inline std::string toString(const SingleAssetValue& singleAssetValue) {
  return std::format("{{baseSymbol={}, quoteAsset={}, value={}}}", singleAssetValue.baseSymbol,
                     singleAssetValue.quoteAsset, singleAssetValue.value);
}

using AssetValues = std::vector<SingleAssetValue>;

}  // namespace MarketService