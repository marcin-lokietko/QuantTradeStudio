#pragma once

#include <nlohmann/json.hpp>

#include "Account/Wallet.hpp"

namespace GuiService::HttpGuiService::Conversion {

nlohmann::json toJson(const ::Account::Wallet& wallet);
}