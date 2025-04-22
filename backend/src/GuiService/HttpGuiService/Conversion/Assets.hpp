#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/Assets.hpp"

namespace GuiService::HttpGuiService::Conversion {

nlohmann::json toJson(const ApiGateway::Assets& assets);
}