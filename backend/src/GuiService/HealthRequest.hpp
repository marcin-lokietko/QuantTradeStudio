#pragma once

#include <string>

#include "Utils/StrongType.hpp"

namespace GuiService {

DEFINE_STRONG_TYPE(Message, std::string);

// HealthRequest.Message is only logged and not used in the system.
// The only purpose is to help with identifying the currently executed test scenario in the logs. Every E2E/component
// test is sending the request with scenario name at the beginning of the scenario
struct HealthRequest {
  Message message;
};

}  // namespace GuiService