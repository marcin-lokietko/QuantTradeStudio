#include "AvailableQuoteAssets.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
nlohmann::json toJson(const ApiGateway::SingleAvailableQuoteAsset& singleAvailableQuoteAsset) {
  nlohmann::json j;
  j = nlohmann::json{{"quoteAsset", singleAvailableQuoteAsset.quoteAsset.val_},
                     {"baseAssetUnitPrice", singleAvailableQuoteAsset.baseAssetUnitPrice.val_}};
  return j;
}
}  // namespace

nlohmann::json toJson(const ApiGateway::AvailableQuoteAssets& availableQuoteAssets) {
  nlohmann::json j = nlohmann::json::array();
  for (const auto& singleAvailableQuoteAsset : availableQuoteAssets) {
    j.push_back(toJson(singleAvailableQuoteAsset));
  }
  return j;
}

}  // namespace GuiService::HttpGuiService::Conversion
