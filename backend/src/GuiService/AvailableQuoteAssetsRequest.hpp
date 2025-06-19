#pragma once

#include <string>

#include "ApiGateway/AssetSymbol.hpp"

namespace GuiService {

struct AvailableQuoteAssetsRequest {
  ApiGateway::AssetSymbol baseAsset;

  bool operator==(const AvailableQuoteAssetsRequest& other) const = default;
};

}  // namespace GuiService