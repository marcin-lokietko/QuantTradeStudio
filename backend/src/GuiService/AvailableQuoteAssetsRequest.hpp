#pragma once

#include <string>

#include "ApiGateway/AssetSymbol.hpp"

namespace GuiService {

struct AvailableQuoteAssetsRequest {
  ApiGateway::AssetSymbol baseAsset;
};

}  // namespace GuiService