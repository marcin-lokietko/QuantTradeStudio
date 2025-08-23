#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/BacktestConfig.hpp"

namespace GuiService::HttpGuiService::Conversion {

void fromJson(const nlohmann::json& j, ApiGateway::BacktestConfig& backtestConfig);

}