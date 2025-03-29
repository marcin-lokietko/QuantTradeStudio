#pragma once

#include "Account/IAccount.hpp"
#include "GuiService/IGuiService.hpp"

namespace GuiService::HttpGuiService {

class HttpGuiService : public IGuiService {
 public:
  HttpGuiService(const Account::IAccount& account) : account_(account){};

  void start() override;

 private:
  const Account::IAccount& account_;
};

}  // namespace GuiService::HttpGuiService