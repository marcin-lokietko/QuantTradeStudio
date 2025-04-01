#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Utils/StrongType.hpp"

namespace Account {

DEFINE_STRONG_TYPE(Asset, std::string);
DEFINE_STRONG_TYPE(FreeAssetAmount, std::string);
DEFINE_STRONG_TYPE(ValueInUsdt, std::string);

struct WalletItem {
  Asset asset{};
  FreeAssetAmount freeAmount{};
  ValueInUsdt usdtValue{};
};

using Wallet = std::vector<WalletItem>;

}  // namespace Account