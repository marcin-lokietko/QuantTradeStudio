#include "OrderRequest.hpp"

namespace GuiService::HttpGuiService::Conversion {

void fromJson(const nlohmann::json& j, OrderRequest& orderRequest) {
  orderRequest = {.assetToBuy = ApiGateway::AssetSymbol{j.at("assetToBuy").get<std::string>()},
                  .assetToSpend = ApiGateway::AssetSymbol{j.at("assetToSpend").get<std::string>()},
                  .quantityToBuy = ApiGateway::AssetQuantity{j.at("quantityToBuy").get<std::string>()}

  };
}

}  // namespace GuiService::HttpGuiService::Conversion