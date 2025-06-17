#include "AvailableBaseAssetsRequest.hpp"

namespace GuiService::HttpGuiService::Conversion {

void fromQueryParams(const crow::query_string& query_params, AvailableBaseAssetsRequest& availableBaseAssetsRequest) {
  availableBaseAssetsRequest.quoteAsset = std::nullopt;
  if (query_params.get("quoteAsset")) {
    availableBaseAssetsRequest.quoteAsset = ApiGateway::AssetSymbol{query_params.get("quoteAsset")};
  }
}

}  // namespace GuiService::HttpGuiService::Conversion