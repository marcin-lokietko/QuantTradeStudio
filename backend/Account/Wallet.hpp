#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Account {

struct WalletItem {
  std::string asset{};
  std::string amountFree{};
  std::string usdtValue{};
};

using Wallet = std::vector<WalletItem>;

}  // namespace Account