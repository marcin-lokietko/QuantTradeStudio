#pragma once

#include <memory>

#include "../StockMarketService/IStockMarketService.h"
#include "IGuiService.hpp"

namespace GuiService {

class ServiceFactory {
 public:
  std::unique_ptr<IGuiService> makeGuiService(const StockMarketService::IStockMarketService& stockMarketService);
};

}  // namespace GuiService