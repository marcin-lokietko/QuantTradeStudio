#include "KlineSequence.hpp"

namespace MarketService::Binance::Conversion {

void fromJson(const nlohmann::json& j, MarketService::KlineSequence& klineSequence) {
  klineSequence.clear();
  klineSequence.reserve(j.size());

  for (const auto& singleKline : j) {
    // Assuming the times are sent in ms since Unix epoch. The unit is not clear from the API spec
    const auto openTime =
        std::chrono::system_clock::time_point(std::chrono::milliseconds(singleKline[0].get<uint64_t>()));
    const auto closeTime =
        std::chrono::system_clock::time_point(std::chrono::milliseconds(singleKline[6].get<uint64_t>()));

    klineSequence.emplace_back(openTime,                                              // Open time
                               closeTime,                                             // Close time
                               ApiGateway::Price{singleKline[1].get<std::string>()},  // Open price
                               ApiGateway::Price{singleKline[4].get<std::string>()},  // Close price
                               ApiGateway::Price{singleKline[3].get<std::string>()},  // Low price
                               ApiGateway::Price{singleKline[2].get<std::string>()}   // High price
    );
  }
}

}  // namespace MarketService::Binance::Conversion