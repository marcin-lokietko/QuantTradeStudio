#include <string>

#include "GuiService.hpp"
#include "ServiceFactory.hpp"

namespace GuiService {

std::unique_ptr<IGuiService> ServiceFactory::makeGuiService(
    const StockMarketService::IStockMarketService& stockMarketService, const std::string& logsCatalog) {
  return std::make_unique<GuiService>(stockMarketService, logsCatalog);
}

}  // namespace GuiService