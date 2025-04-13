#pragma once

#include <optional>

#include "ApiGateway/AssetSymbol.hpp"

namespace GuiService {

struct AvailableBaseAssetsRequest {
  std::optional<ApiGateway::AssetSymbol> quoteAsset;
};

}  // namespace GuiService