#pragma once

#include <optional>

#include "ConfigParams.hpp"
#include "Utils/StrongType.hpp"

namespace Config {

std::optional<ConfigParams> readConfig(const std::filesystem::path& configPath);

}  // namespace Config