#include "BacktestConfig.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
void fromJson(const nlohmann::json& j, ApiGateway::AssetQuantities& assetQuantities) {
  for (const auto& jsonItem : j) {
    assetQuantities.push_back(ApiGateway::SingleAssetQuantity{
        ApiGateway::AssetSymbol{jsonItem.at("assetSymbol").get<std::string>()},
        ApiGateway::AssetQuantity{jsonItem.at("freeQuantity").get<std::string>()}, ApiGateway::MarketId::Unknown});
  }
}
}  // namespace

void fromJson(const nlohmann::json& j, ApiGateway::BacktestConfig& backtestConfig) {
  backtestConfig = {};

  backtestConfig.transactionFeePercent = ApiGateway::TransactionFeePercent{j.at("transactionFeePercent").get<double>()};

  ApiGateway::AssetQuantities initialOwnedAssets;
  fromJson(j.at("initialOwnedAssets"), initialOwnedAssets);
  backtestConfig.initialOwnedAssets = initialOwnedAssets;

  backtestConfig.simulationStart =
      std::chrono::system_clock::time_point{std::chrono::milliseconds{j.at("simulationStart").get<int64_t>()}};
  backtestConfig.simulationEnd =
      std::chrono::system_clock::time_point{std::chrono::milliseconds{j.at("simulationEnd").get<int64_t>()}};
}

}  // namespace GuiService::HttpGuiService::Conversion