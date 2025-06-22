#include "KlineSequence.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::KlineSequence& klineSequence) {
  klineSequence.clear();
  klineSequence.reserve(j.size());

  for (const auto& singleKline : j) {
    klineSequence.emplace_back(Time{std::to_string(singleKline[6].get<uint64_t>())},
                               ApiGateway::Price{singleKline[4].get<std::string>()});
  }
}

}  // namespace MarketService::Binance::Conversion