#include "GuiService.hpp"
#include "ServiceFactory.hpp"

namespace GuiService {

std::unique_ptr<IGuiService> ServiceFactory::makeGuiService(
    const StockMarketService::IStockMarketService& stockMarketService) {
  return std::make_unique<GuiService>(stockMarketService);
}

}  // namespace GuiService