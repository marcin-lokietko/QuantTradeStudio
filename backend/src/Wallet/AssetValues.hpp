#pragma once

#include <optional>
#include <vector>

#include "ApiGateway/AssetSymbol.hpp"
#include "ApiGateway/Value.hpp"

namespace Wallet {

struct SingleAssetValue {
  ApiGateway::AssetSymbol baseSymbol{};
  ApiGateway::AssetSymbol quoteAsset{};
  std::optional<ApiGateway::Value> value{};
};

using AssetValues = std::vector<SingleAssetValue>;

}  // namespace Wallet