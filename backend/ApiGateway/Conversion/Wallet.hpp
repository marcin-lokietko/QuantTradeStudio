#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Account/Wallet.hpp"
#include "ApiGateway/Wallet.hpp"

namespace ApiGateway::Conversion {

Wallet toApi(const Account::Wallet& wallet);

}  // namespace ApiGateway::Conversion