#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/AvailableQuoteAssets.hpp"

namespace GuiService::HttpGuiService::Conversion {

nlohmann::json toJson(const ApiGateway::AvailableQuoteAssets& availableQuoteAssets);
}