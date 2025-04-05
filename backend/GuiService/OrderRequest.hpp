#pragma once

#include <string>

#include "ApiGateway/AssetQuantity.hpp"
#include "ApiGateway/AssetSymbol.hpp"

namespace GuiService {

struct OrderRequest {
  ApiGateway::AssetSymbol assetToBuy;
  ApiGateway::AssetSymbol assetToSpend;
  ApiGateway::AssetQuantity quantityToBuy;
};

}  // namespace GuiService