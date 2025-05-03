#pragma once

#include <nlohmann/json.hpp>

#include "Config/ConfigParams.hpp"

namespace Config::Conversion {

void fromJson(const nlohmann::json& j, Config::ConfigParams& configParams);

}