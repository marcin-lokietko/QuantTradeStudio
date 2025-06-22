#pragma once

#include <chrono>
#include <vector>

#include "ApiGateway/AssetValues.hpp"
#include "Utils/StrongType.hpp"

namespace ApiGateway {

using AssetQuantitiesAndValuesHistory = std::vector<std::pair<std::chrono::system_clock::time_point, AssetValues>>;

}  // namespace ApiGateway