#include <string>

#include "../Account/IAccount.h"
#include "GuiService.hpp"
#include "ServiceFactory.hpp"

namespace GuiService {

std::unique_ptr<IGuiService> ServiceFactory::makeGuiService(const Account::IAccount& account) {
  return std::make_unique<GuiService>(account);
}

}  // namespace GuiService