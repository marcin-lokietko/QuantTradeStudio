#pragma once

#include "Account/IAccount.h"
#include "IGuiService.hpp"

namespace GuiService {

class GuiService : public IGuiService {
 public:
  GuiService(const Account::IAccount& account) : account_(account){};

  void start() override;

 private:
  const Account::IAccount& account_;
};

}  // namespace GuiService