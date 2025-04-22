#pragma once

#include <string>

#include "ApiGateway/AssetQuantity.hpp"
#include "ApiGateway/AssetSymbol.hpp"
#include "ApiGateway/OrderSide.hpp"

namespace GuiService {

struct OrderRequest {
  ApiGateway::AssetSymbol selectedBaseAsset;
  ApiGateway::AssetSymbol selectedQuoteAsset;
  ApiGateway::OrderSide orderSide;
  ApiGateway::AssetQuantity baseAssetAmount;
};

}  // namespace GuiService