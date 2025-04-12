#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/AssetSymbols.hpp"

namespace GuiService::HttpGuiService::Conversion {

nlohmann::json toJson(const ApiGateway::AssetSymbols& assetSymbols);
}