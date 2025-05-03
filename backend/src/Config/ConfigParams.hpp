#pragma once

#include <filesystem>
#include <string>

#include "Utils/StrongType.hpp"

namespace Config {

DEFINE_STRONG_TYPE(LogsCatalogPath, std::filesystem::path);
DEFINE_STRONG_TYPE(KeysCatalogPath, std::filesystem::path);
DEFINE_STRONG_TYPE(BinanceUrlPrefix, std::string);

struct ConfigParams {
  LogsCatalogPath logsCatalogPath{};
  KeysCatalogPath keysCatalogPath{};
  BinanceUrlPrefix binanceUrlPrefix{};
};

}  // namespace Config