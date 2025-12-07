#include "BotConfig.hpp"

namespace GuiService::HttpGuiService::Conversion {

namespace {
void fromJson(const nlohmann::json& j, ApiGateway::AssetShares& assetShare) {
  for (const auto& jsonItem : j) {
    assetShare.push_back(
        ApiGateway::SingleAssetShare{ApiGateway::AssetSymbol{jsonItem.at("assetSymbol").get<std::string>()},
                                     ApiGateway::SharePercent{jsonItem.at("expectedShare").get<uint64_t>()}});
  }
}

ApiGateway::KlineInterval toKlineInterval(const std::string& klineIntervalString) {
  if (klineIntervalString == "OneMinute") {
    return ApiGateway::KlineInterval::OneMinute;
  }
  if (klineIntervalString == "ThreeMinutes") {
    return ApiGateway::KlineInterval::ThreeMinutes;
  }
  if (klineIntervalString == "FiveMinutes") {
    return ApiGateway::KlineInterval::FiveMinutes;
  }
  if (klineIntervalString == "FifteenMinutes") {
    return ApiGateway::KlineInterval::FifteenMinutes;
  }
  if (klineIntervalString == "ThirtyMinutes") {
    return ApiGateway::KlineInterval::ThirtyMinutes;
  }
  if (klineIntervalString == "OneHour") {
    return ApiGateway::KlineInterval::OneHour;
  }
  if (klineIntervalString == "TwoHours") {
    return ApiGateway::KlineInterval::TwoHours;
  }
  if (klineIntervalString == "FourHours") {
    return ApiGateway::KlineInterval::FourHours;
  }
  if (klineIntervalString == "SixHours") {
    return ApiGateway::KlineInterval::SixHours;
  }
  if (klineIntervalString == "EightHours") {
    return ApiGateway::KlineInterval::EightHours;
  }
  if (klineIntervalString == "TwelveHours") {
    return ApiGateway::KlineInterval::TwelveHours;
  }
  if (klineIntervalString == "OneDay") {
    return ApiGateway::KlineInterval::OneDay;
  }
  if (klineIntervalString == "ThreeDays") {
    return ApiGateway::KlineInterval::ThreeDays;
  }
  if (klineIntervalString == "OneWeek") {
    return ApiGateway::KlineInterval::OneWeek;
  }
  if (klineIntervalString == "OneMonth") {
    return ApiGateway::KlineInterval::OneMonth;
  }
  return ApiGateway::KlineInterval::Invalid;
}
}  // namespace

void fromJson(const nlohmann::json& j, ApiGateway::BotConfig& botConfig) {
  botConfig = {};
  botConfig.botName = ApiGateway::BotName{j.at("botName").get<std::string>()};

  if (j.contains("quoteAsset")) {
    botConfig.quoteAsset = ApiGateway::AssetSymbol{j.at("quoteAsset").get<std::string>()};
  }
  if (j.contains("executionPeriod")) {
    botConfig.executionPeriod = ApiGateway::ExecutionPeriod{j.at("executionPeriod").get<uint64_t>()};
  }
  if (j.contains("isExecutedImmediately")) {
    botConfig.isExecutedImmediately = ApiGateway::IsExecutedImmediately{j.at("isExecutedImmediately").get<bool>()};
  }
  if (j.contains("baseAssetShares")) {
    ApiGateway::AssetShares baseAssetShares;
    fromJson(j.at("baseAssetShares"), baseAssetShares);
    botConfig.baseAssetShares = baseAssetShares;
  }
  if (j.contains("shortTermMovingAverageLength")) {
    botConfig.shortTermMovingAverageLength =
        ApiGateway::AssetHistoryLength{j.at("shortTermMovingAverageLength").get<uint64_t>()};
  }
  if (j.contains("longTermMovingAverageLength")) {
    botConfig.longTermMovingAverageLength =
        ApiGateway::AssetHistoryLength{j.at("longTermMovingAverageLength").get<uint64_t>()};
  }
  if (j.contains("baseAssets")) {
    ApiGateway::AssetSymbols baseAssets;
    for (const auto& jsonItem : j.at("baseAssets")) {
      baseAssets.push_back(ApiGateway::AssetSymbol{jsonItem.get<std::string>()});
    }
    botConfig.baseAssets = baseAssets;
  }
  if (j.contains("executionInterval")) {
    botConfig.executionInterval = toKlineInterval(j.at("executionInterval").get<std::string>());
  }
  if (j.contains("entryChannelLength")) {
    botConfig.entryChannelLength = ApiGateway::AssetHistoryLength{j.at("entryChannelLength").get<uint64_t>()};
  }
  if (j.contains("exitChannelLength")) {
    botConfig.exitChannelLength = ApiGateway::AssetHistoryLength{j.at("exitChannelLength").get<uint64_t>()};
  }
}

}  // namespace GuiService::HttpGuiService::Conversion