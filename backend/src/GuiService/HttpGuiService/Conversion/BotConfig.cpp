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
}  // namespace

void fromJson(const nlohmann::json& j, ApiGateway::BotConfig& botConfig) {
  botConfig = {};
  botConfig.botName = ApiGateway::BotName{j.at("botName").get<std::string>()};

  if (j.contains("executionPeriod")) {
    botConfig.executionPeriod = ApiGateway::ExecutionPeriod{j.at("executionPeriod").get<uint64_t>()};
  }
  if (j.contains("isExecutedImmediately")) {
    botConfig.isExecutedImmediately = ApiGateway::IsExecutedImmediately{j.at("isExecutedImmediately").get<bool>()};
  }
  if (j.contains("quoteAsset")) {
    botConfig.quoteAsset = ApiGateway::AssetSymbol{j.at("quoteAsset").get<std::string>()};
  }
  if (j.contains("baseAssetShares")) {
    ApiGateway::AssetShares baseAssetShares;
    fromJson(j.at("baseAssetShares"), baseAssetShares);
    botConfig.baseAssetShares = baseAssetShares;
  }
}

}  // namespace GuiService::HttpGuiService::Conversion