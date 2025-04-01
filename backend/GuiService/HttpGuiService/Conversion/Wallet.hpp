#pragma once

#include <nlohmann/json.hpp>

#include "ApiGateway/Wallet.hpp"

namespace GuiService::HttpGuiService::Conversion {

nlohmann::json toJson(const ApiGateway::Wallet& wallet);
}