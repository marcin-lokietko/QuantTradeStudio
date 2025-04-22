#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/BotConfig.hpp"

namespace GuiService::HttpGuiService::Conversion {

void fromJson(const nlohmann::json& j, ApiGateway::BotConfig& botConfig);

}