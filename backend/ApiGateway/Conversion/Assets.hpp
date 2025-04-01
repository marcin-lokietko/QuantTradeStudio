#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ApiGateway/Assets.hpp"
#include "Wallet/Assets.hpp"

namespace ApiGateway::Conversion {

Assets toApi(const Wallet::Assets& assets);

}  // namespace ApiGateway::Conversion