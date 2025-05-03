#include "ConfigParams.hpp"

namespace Config::Conversion {

void fromJson(const nlohmann::json& j, Config::ConfigParams& configParams) {
  configParams = {.logsCatalogPath = Config::LogsCatalogPath{j.at("logsCatalogPath").get<std::string>()},
                  .keysCatalogPath = Config::KeysCatalogPath{j.at("keysCatalogPath").get<std::string>()},
                  .binanceUrlPrefix = Config::BinanceUrlPrefix{j.at("binanceUrlPrefix").get<std::string>()}

  };
}

}  // namespace Config::Conversion