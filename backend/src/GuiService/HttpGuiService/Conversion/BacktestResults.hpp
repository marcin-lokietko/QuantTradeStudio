#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/BacktestResults.hpp"

namespace GuiService::HttpGuiService::Conversion {

nlohmann::json toJson(const ApiGateway::BacktestResults& backtestResults);
}