#include <curl/curl.h>
#include <glog/logging.h>

#include <filesystem>
#include <nlohmann/json.hpp>

void setupLogger(const char* programName, const char* logDir) {
  std::filesystem::create_directory(logDir);
  FLAGS_log_dir = logDir;
  FLAGS_stderrthreshold = 0;

  google::InitGoogleLogging(programName);

  LOG(INFO) << "Glog logger successfully initialized";
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  const size_t contentSizeBytes = size * nmemb;
  static_cast<std::string*>(userp)->append(static_cast<char*>(contents), contentSizeBytes);
  return contentSizeBytes;
}

std::string getBinancePrice(const std::string& symbol) {
  std::string receivedData;
  std::string url = "https://api.binance.com/api/v3/ticker/price?symbol=" + symbol;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  const auto curl = curl_easy_init();

  if (!curl) {
    curl_global_cleanup();
  }
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);

  const auto res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    LOG(ERROR) << "Failed to receive price, error: " << curl_easy_strerror(res);
  }

  curl_easy_cleanup(curl);
  return receivedData;
}

int main(int, char* argv[]) {
  setupLogger(argv[0], argv[1]);

  LOG(INFO) << "########## Starting AlgoTrader";

  const std::string symbol = "BTCUSDT";
  const std::string response = getBinancePrice(symbol);

  const auto jsonResponse = nlohmann::json::parse(response);
  if (jsonResponse.contains("price")) {
    const std::string price = jsonResponse["price"];
    LOG(INFO) << "Current price of " << symbol << ": " << price;
  } else {
    LOG(ERROR) << "Failed to retrieve price.";
  }

  LOG(INFO) << "########## Ending AlgoTrader";
}
