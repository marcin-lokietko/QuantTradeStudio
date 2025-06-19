#include "Config/Conversion/ConfigParams.hpp"

#include "gmock/gmock.h"

namespace Config::Conversion {

TEST(ConfigParamsTest, configJsonIsConvertedToConfigParamsStructure) {
  nlohmann::json inputJson = {{"logsCatalogPath", "path/to/logs"},
                              {"keysCatalogPath", "path/to/keys"},
                              {"binanceUrlPrefix", "https://testnet.binance.vision/api/v3"}};
  Config::ConfigParams outputConfigParams;
  fromJson(inputJson, outputConfigParams);
  EXPECT_EQ(outputConfigParams.logsCatalogPath.val_, "path/to/logs");
  EXPECT_EQ(outputConfigParams.keysCatalogPath.val_, "path/to/keys");
  EXPECT_EQ(outputConfigParams.binanceUrlPrefix.val_, "https://testnet.binance.vision/api/v3");
}
}  // namespace Config::Conversion
