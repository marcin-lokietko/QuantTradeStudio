#include "AvailableQuoteAssetsRequest.hpp"

namespace GuiService::HttpGuiService::Conversion {

void fromQueryParams(const crow::query_string& query_params,
                     std::optional<AvailableQuoteAssetsRequest>& availableQuoteAssetsRequest) {
  if (!query_params.get("baseAsset")) {
    availableQuoteAssetsRequest = std::nullopt;
    return;
  }
  availableQuoteAssetsRequest = {.baseAsset = ApiGateway::AssetSymbol{query_params.get("baseAsset")}};
}

}  // namespace GuiService::HttpGuiService::Conversion