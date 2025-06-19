#pragma once

#include <optional>

#include "ApiGateway/AssetSymbol.hpp"

namespace GuiService {

struct AvailableBaseAssetsRequest {
  std::optional<ApiGateway::AssetSymbol> quoteAsset;

  bool operator==(const AvailableBaseAssetsRequest& other) const = default;
};

}  // namespace GuiService