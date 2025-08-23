#include "BacktestResults.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
nlohmann::json toJson(const ApiGateway::AssetQuantitiesAndValuesHistory& assetQuantitiesAndValuesHistory) {
  nlohmann::json j;

  for (const auto& [timePoint, assetValues] : assetQuantitiesAndValuesHistory) {
    nlohmann::json entry;

    // time_point is sent as milliseconds since Unix epoch
    auto duration = timePoint.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    entry["timestamp"] = millis;

    nlohmann::json assets = nlohmann::json::array();
    for (const auto& singleAssetValue : assetValues) {
      nlohmann::json asset;
      asset["assetSymbol"] = singleAssetValue.assetSymbol.val_;
      asset["freeQuantity"] = singleAssetValue.freeQuantity.val_;
      asset["usdtValue"] = singleAssetValue.usdtValue.val_;
      assets.push_back(asset);
    }
    entry["assets"] = assets;

    j.push_back(entry);
  }

  return j;
}
}  // namespace

nlohmann::json toJson(const ApiGateway::BacktestResults& backtestResults) {
  nlohmann::json j;
  if (backtestResults.botAssetHistory) {
    j["botAssetHistory"] = toJson(backtestResults.botAssetHistory.value());
  }
  if (backtestResults.totalProfitOrLossInAbsolute) {
    j["totalProfitOrLossInAbsolute"] = backtestResults.totalProfitOrLossInAbsolute->val_;
  }
  if (backtestResults.totalProfitOrLossInPercent) {
    j["totalProfitOrLossInPercent"] = backtestResults.totalProfitOrLossInPercent->val_;
  }
  if (backtestResults.winRate) {
    j["winRate"] = backtestResults.winRate->val_;
  }
  if (backtestResults.averageWinSizeInAbsolute) {
    j["averageWinSizeInAbsolute"] = backtestResults.averageWinSizeInAbsolute->val_;
  }
  if (backtestResults.averageLossSizeInAbsolute) {
    j["averageLossSizeInAbsolute"] = backtestResults.averageLossSizeInAbsolute->val_;
  }
  if (backtestResults.assetHistoryIfHeld) {
    j["assetHistoryIfHeld"] = toJson(backtestResults.assetHistoryIfHeld.value());
  }
  if (backtestResults.totalProfitOrLossInAbsoluteIfHeld) {
    j["totalProfitOrLossInAbsoluteIfHeld"] = backtestResults.totalProfitOrLossInAbsoluteIfHeld->val_;
  }
  if (backtestResults.totalProfitOrLossInPercentIfHeld) {
    j["totalProfitOrLossInPercentIfHeld"] = backtestResults.totalProfitOrLossInPercentIfHeld->val_;
  }
  if (backtestResults.absoluteAsset) {
    j["absoluteAsset"] = backtestResults.absoluteAsset->val_;
  }
  return j;
}

}  // namespace GuiService::HttpGuiService::Conversion