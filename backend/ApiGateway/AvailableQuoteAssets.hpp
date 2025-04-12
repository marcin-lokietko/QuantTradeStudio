#pragma once

#include <vector>

#include "AssetSymbol.hpp"
#include "Price.hpp"

namespace ApiGateway {

struct SingleAvailableQuoteAsset {
  AssetSymbol quoteAsset{};
  Price baseAssetUnitPrice{};
};

using AvailableQuoteAssets = std::vector<SingleAvailableQuoteAsset>;

}  // namespace ApiGateway