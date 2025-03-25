#pragma once

#include "../StockMarketService/IStockMarketService.h"
#include "IGuiService.hpp"

namespace GuiService {

class GuiService : public IGuiService {
 public:
  GuiService(const StockMarketService::IStockMarketService& stockMarketService, const std::string& logsCatalog)
      : stockMarketService_(stockMarketService), logsCatalog_(logsCatalog){};

  void start() override;

 private:
  const StockMarketService::IStockMarketService& stockMarketService_;
  const std::string logsCatalog_;
};

}  // namespace GuiService