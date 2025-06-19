#include "GuiService/HttpGuiService/Conversion/AvailableBaseAssetsRequest.hpp"

#include "gmock/gmock.h"

namespace GuiService::HttpGuiService::Conversion {

TEST(AvailableBaseAssetsRequestTest, converts) {
  const crow::query_string availableBaseAssetsRequestQueryParams{"?quoteAsset=USDT"};
  GuiService::AvailableBaseAssetsRequest actualRequest;
  fromQueryParams(availableBaseAssetsRequestQueryParams, actualRequest);
  const GuiService::AvailableBaseAssetsRequest expectedRequest{ApiGateway::AssetSymbol{"USDT"}};
  EXPECT_EQ(expectedRequest, actualRequest);
}

TEST(AvailableBaseAssetsRequestTest, convertsNullopt) {
  const crow::query_string availableBaseAssetsRequestQueryParams{""};
  GuiService::AvailableBaseAssetsRequest actualRequest;
  fromQueryParams(availableBaseAssetsRequestQueryParams, actualRequest);
  const GuiService::AvailableBaseAssetsRequest expectedRequest{std::nullopt};
  EXPECT_EQ(expectedRequest, actualRequest);
}
}  // namespace GuiService::HttpGuiService::Conversion
