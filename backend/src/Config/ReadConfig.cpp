#include <fstream>

#include "Config/Conversion/ConfigParams.hpp"
#include "ReadConfig.hpp"

namespace Config {
std::optional<ConfigParams> readConfig(const std::filesystem::path& configPath) {
  std::ifstream file(configPath);
  if (!file) {
    return std::nullopt;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string content = buffer.str();

  ConfigParams config;
  Conversion::fromJson(nlohmann::json::parse(content), config);

  return config;
}

}  // namespace Config