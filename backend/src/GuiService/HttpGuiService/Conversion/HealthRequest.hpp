#pragma once

#include <nlohmann/json.hpp>

#include "GuiService/HealthRequest.hpp"
#include "crow.h"

namespace GuiService::HttpGuiService::Conversion {

void fromQueryParams(const crow::query_string& query_params, std::optional<HealthRequest>& healthRequest);

}