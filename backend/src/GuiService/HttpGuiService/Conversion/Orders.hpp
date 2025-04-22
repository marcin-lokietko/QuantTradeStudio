#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/Orders.hpp"

namespace GuiService::HttpGuiService::Conversion {

nlohmann::json toJson(const ApiGateway::Orders& orders);
}