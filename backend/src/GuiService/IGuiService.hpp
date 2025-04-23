#pragma once

namespace GuiService {

class IGuiService {
 public:
  virtual ~IGuiService() = default;

  virtual void start() = 0;
};

}  // namespace GuiService