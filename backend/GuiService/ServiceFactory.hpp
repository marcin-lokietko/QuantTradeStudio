#pragma once

#include <memory>

#include "IGuiService.hpp"

namespace GuiService {

class ServiceFactory {
 public:
  std::unique_ptr<IGuiService> makeGuiService();
};

}  // namespace GuiService