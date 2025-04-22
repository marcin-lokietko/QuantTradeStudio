#pragma once

#include <nlohmann/json.hpp>

#include "GuiService/AvailableQuoteAssetsRequest.hpp"
#include "crow.h"

namespace GuiService::HttpGuiService::Conversion {

void fromQueryParams(const crow::query_string& query_params,
                     std::optional<AvailableQuoteAssetsRequest>& availableQuoteAssetsRequest);

}