#include "GuiService.hpp"
#include "ServiceFactory.hpp"

namespace GuiService {

std::unique_ptr<IGuiService> ServiceFactory::makeGuiService() { return std::make_unique<GuiService>(); }

}  // namespace GuiService