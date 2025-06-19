#include "GuiService/HttpGuiService/Conversion/AvailableQuoteAssetsRequest.hpp"

#include "gmock/gmock.h"

namespace GuiService::HttpGuiService::Conversion {

TEST(AvailableQuoteAssetsRequestTest, converts) {
  const crow::query_string availableQuoteAssetsRequestQueryParams{"?baseAsset=USDT"};
  std::optional<GuiService::AvailableQuoteAssetsRequest> actualRequest;
  fromQueryParams(availableQuoteAssetsRequestQueryParams, actualRequest);
  const std::optional<GuiService::AvailableQuoteAssetsRequest> expectedRequest{ApiGateway::AssetSymbol{"USDT"}};
  EXPECT_EQ(expectedRequest, actualRequest);
}

TEST(AvailableQuoteAssetsRequestTest, convertsNullopt) {
  const crow::query_string availableQuoteAssetsRequestQueryParams{""};
  std::optional<GuiService::AvailableQuoteAssetsRequest> actualRequest;
  fromQueryParams(availableQuoteAssetsRequestQueryParams, actualRequest);
  EXPECT_EQ(std::nullopt, actualRequest);
}
}  // namespace GuiService::HttpGuiService::Conversion
