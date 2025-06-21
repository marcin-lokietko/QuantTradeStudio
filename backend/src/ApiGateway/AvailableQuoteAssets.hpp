#pragma once

#include <format>
#include <vector>

#include "AssetSymbol.hpp"
#include "Price.hpp"

namespace ApiGateway {

struct SingleAvailableQuoteAsset {
  AssetSymbol quoteAsset{};
  Price baseAssetUnitPrice{};

  bool operator==(const SingleAvailableQuoteAsset& other) const = default;
};

using AvailableQuoteAssets = std::vector<SingleAvailableQuoteAsset>;

inline std::string toString(const SingleAvailableQuoteAsset& singleAvailableQuoteAsset) {
  return std::format("{{quoteAsset={}, baseAssetUnitPrice={}}}", singleAvailableQuoteAsset.quoteAsset.val_,
                     singleAvailableQuoteAsset.baseAssetUnitPrice.val_);
}

}  // namespace ApiGateway