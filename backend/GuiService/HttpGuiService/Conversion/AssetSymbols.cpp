#include "AssetSymbols.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
nlohmann::json toJson(const ApiGateway::AssetSymbol& assetSymbol) {
  nlohmann::json j;
  j = nlohmann::json{{"baseAsset", assetSymbol.val_}};
  return j;
}
}  // namespace

nlohmann::json toJson(const ApiGateway::AssetSymbols& assetSymbols) {
  nlohmann::json j = nlohmann::json::array();
  for (const auto& singleAssetSymbol : assetSymbols) {
    j.push_back(toJson(singleAssetSymbol));
  }
  return j;
}

}  // namespace GuiService::HttpGuiService::Conversion