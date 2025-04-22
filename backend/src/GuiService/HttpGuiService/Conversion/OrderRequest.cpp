#include "OrderRequest.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
ApiGateway::OrderSide toOrderSide(std::string orderSideString) {
  if (orderSideString == "Buy") {
    return ApiGateway::OrderSide::Buy;
  }
  if (orderSideString == "Sell") {
    return ApiGateway::OrderSide::Sell;
  }
  return ApiGateway::OrderSide::Invalid;
}
}  // namespace

void fromJson(const nlohmann::json& j, OrderRequest& orderRequest) {
  orderRequest = {.selectedBaseAsset = ApiGateway::AssetSymbol{j.at("selectedBaseAsset").get<std::string>()},
                  .selectedQuoteAsset = ApiGateway::AssetSymbol{j.at("selectedQuoteAsset").get<std::string>()},
                  .orderSide = toOrderSide(j.at("orderSide").get<std::string>()),
                  .baseAssetAmount = ApiGateway::AssetQuantity{j.at("baseAssetAmount").get<std::string>()}

  };
}

}  // namespace GuiService::HttpGuiService::Conversion