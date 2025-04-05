#pragma once

#include <nlohmann/json.hpp>

#include "GuiService/OrderRequest.hpp"

namespace GuiService::HttpGuiService::Conversion {

void fromJson(const nlohmann::json& j, OrderRequest& orderRequest);

}