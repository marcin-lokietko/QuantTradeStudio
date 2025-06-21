#include "HealthRequest.hpp"

namespace GuiService::HttpGuiService::Conversion {

void fromQueryParams(const crow::query_string& query_params, std::optional<HealthRequest>& healthRequest) {
  if (!query_params.get("message")) {
    healthRequest = std::nullopt;
    return;
  }
  healthRequest = {.message = GuiService::Message{query_params.get("message")}};
}

}  // namespace GuiService::HttpGuiService::Conversion