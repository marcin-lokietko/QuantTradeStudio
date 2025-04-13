#include "AvailableBaseAssetsRequest.hpp"

namespace GuiService::HttpGuiService::Conversion {

void fromQueryParams(const crow::query_string& query_params, AvailableBaseAssetsRequest& availableBaseAssetsRequest) {
  availableBaseAssetsRequest.quoteAsset = std::nullopt;
  if (query_params.get("assetSymbol")) {
    availableBaseAssetsRequest.quoteAsset = ApiGateway::AssetSymbol{query_params.get("assetSymbol")};
  }
}

}  // namespace GuiService::HttpGuiService::Conversion