#pragma once

#include <vector>

#include "ApiGateway/AssetQuantities.hpp"
#include "Utils/StrongType.hpp"

namespace BotBacktester {

using BotAssetsHistory = std::vector<std::pair<std::chrono::system_clock::time_point, ApiGateway::AssetQuantities>>;

}  // namespace BotBacktester