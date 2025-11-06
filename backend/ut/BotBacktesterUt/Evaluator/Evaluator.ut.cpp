#include "BotBacktester/Evaluator/Evaluator.hpp"
#include "gmock/gmock.h"

namespace BotBacktester::Evaluator {

using testing::_;
using testing::Return;

using ApiGateway::AssetSymbol;
using ApiGateway::TradingPairSymbol;

const TradingPairSymbol btcUsdtTradingPair{AssetSymbol{"BTC"}, AssetSymbol{"USDT"}};
const std::map<TradingPairSymbol, MarketService::KlineSequence> klines;

class EvaluatorTest : public ::testing::Test {
 public:
  Evaluator sut_{klines};
};

TEST_F(EvaluatorTest, WhenXXXCalled_ThenYYYReturned) {}

}  // namespace BotBacktester::Evaluator
