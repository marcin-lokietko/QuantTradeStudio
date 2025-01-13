#pragma once

#include "IGuiService.hpp"

namespace GuiService {

class GuiService : public IGuiService {
  void start() override;
};

}  // namespace GuiService