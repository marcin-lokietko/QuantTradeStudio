#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/AssetValues.hpp"

namespace GuiService::HttpGuiService::Conversion {

nlohmann::json toJson(const ApiGateway::AssetValues& assets);
}