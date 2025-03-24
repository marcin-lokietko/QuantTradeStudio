#pragma once

#include "../StockMarketService/IStockMarketService.h"
#include "IGuiService.hpp"

namespace GuiService {

class GuiService : public IGuiService {
 public:
  GuiService(const StockMarketService::IStockMarketService& stockMarketService)
      : stockMarketService_(stockMarketService){};

  void start() override;

 private:
  const StockMarketService::IStockMarketService& stockMarketService_;
};

}  // namespace GuiService