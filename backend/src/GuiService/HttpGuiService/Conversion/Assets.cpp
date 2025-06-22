#include "Assets.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
nlohmann::json toJson(const ApiGateway::SingleAssetValue& singleAsset) {
  nlohmann::json j;
  j = nlohmann::json{{"assetSymbol", singleAsset.assetSymbol.val_},
                     {"freeQuantity", singleAsset.freeQuantity.val_},
                     {"usdtValue", singleAsset.usdtValue.val_}};
  return j;
}
}  // namespace

nlohmann::json toJson(const ApiGateway::AssetValues& assets) {
  nlohmann::json j = nlohmann::json::array();
  for (const auto& singleAsset : assets) {
    j.push_back(toJson(singleAsset));
  }
  return j;
}

}  // namespace GuiService::HttpGuiService::Conversion