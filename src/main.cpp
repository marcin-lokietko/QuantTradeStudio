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

size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
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
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receivedData);

  const auto res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    LOG(ERROR) << "Failed to receive price, error: " << curl_easy_strerror(res);
  }

  curl_easy_cleanup(curl);
  return receivedData;
}

void printBinancePrice(const std::string& symbol) {
  const std::string response = getBinancePrice(symbol);

  const auto jsonResponse = nlohmann::json::parse(response);
  if (jsonResponse.contains("price")) {
    const std::string price = jsonResponse["price"];
    LOG(INFO) << "Current price of " << symbol << ": " << price;
  } else {
    LOG(ERROR) << "Failed to retrieve price.";
  }
}

std::string runHttpGet(const std::string& url) {
  CURL* curl = curl_easy_init();
  std::string response;

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  return response;
}

int main(int, char* argv[]) {
  setupLogger(argv[0], argv[1]);

  LOG(INFO) << "########## Starting AlgoTrader";

  const std::string symbol = "BTCUSDT";
  printBinancePrice(symbol);

  // Test connectivity with Binance testnet
  const std::string binanceTestnetBaseUrl = "https://testnet.binance.vision";
  const std::string timeUrl = binanceTestnetBaseUrl + "/api/v3/time";
  const std::string timeResponse = runHttpGet(timeUrl);

  LOG(INFO) << "Binance testnet GET /api/v3/time response: " << timeResponse;

  LOG(INFO) << "########## Ending AlgoTrader";
}
