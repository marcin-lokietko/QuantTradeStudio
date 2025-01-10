#include <curl/curl.h>
#include <glog/logging.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

const std::string binanceTestnetBaseUrl = "https://testnet.binance.vision";
const std::string binanceRealBaseUrl = "https://api.binance.com";

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
  std::string url = binanceTestnetBaseUrl + "/api/v3/ticker/price?symbol=" + symbol;

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

std::string runHttpGetWithHeader(const std::string& url, const std::string& header) {
  CURL* curl = curl_easy_init();
  std::string response;

  if (curl) {
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }
  return response;
}

std::string runHttpPostWithHeader(const std::string& url, const std::string& header) {
  CURL* curl = curl_easy_init();
  std::string response;

  const std::string postFields = "";

  if (curl) {
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }
  return response;
}

std::string generateSignature(const std::string& data, const std::string& secretKey) {
  unsigned char* digest;
  digest = HMAC(EVP_sha256(), secretKey.c_str(), secretKey.size(), reinterpret_cast<const unsigned char*>(data.c_str()),
                data.size(), nullptr, nullptr);

  std::ostringstream result;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
  }
  return result.str();
}

std::string getSecretKey(const std::string& keysDir) {
  std::ifstream stream(keysDir + "/secretKey.txt");
  std::string key;
  stream >> key;
  return key;
}
std::string getApiKey(const std::string& keysDir) {
  std::ifstream stream(keysDir + "/apiKey.txt");
  std::string key;
  stream >> key;
  return key;
}

std::string getAccountUrl(const std::string& keysPath) {
  const std::string timestamp = "timestamp=" + std::to_string(time(nullptr) * 1000);
  const std::string signature = generateSignature(timestamp, getSecretKey(keysPath));
  const std::string signedQuery = timestamp + "&signature=" + signature;
  return binanceTestnetBaseUrl + "/api/v3/account?" + signedQuery;
}

std::string getOrderUrl(const std::string& keysPath, const std::string& queryString) {
  const std::string signature = generateSignature(queryString, getSecretKey(keysPath));
  const std::string signedQuery = queryString + "&signature=" + signature;
  return binanceTestnetBaseUrl + "/api/v3/order?" + signedQuery;
}

int main(int, char* argv[]) {
  setupLogger(argv[0], argv[1]);

  LOG(INFO) << "########## Starting AlgoTrader";

  // Test connectivity with Binance testnet
  const std::string timeUrl = binanceTestnetBaseUrl + "/api/v3/time";
  const std::string timeResponse = runHttpGet(timeUrl);
  LOG(INFO) << "Binance testnet GET /api/v3/time response: " << timeResponse;

  // Print price
  const std::string symbol = "BTCUSDT";
  printBinancePrice(symbol);

  // Fetch klines
  const std::string interval = "1h";
  const std::string klinesUrl =
      binanceTestnetBaseUrl + "/api/v3/klines?symbol=" + symbol + "&interval=" + interval + "&limit=1000";
  const std::string klinesResponse = runHttpGet(klinesUrl);
  const auto klinesJson = nlohmann::json::parse(klinesResponse);
  std::ofstream fileKlines(std::string(argv[1]) + "/klines.txt");
  fileKlines << klinesJson.dump(4);

  // Print Binance testnet account details
  const auto accountUrl = getAccountUrl(argv[2]);
  std::string accountResponse = runHttpGetWithHeader(accountUrl, "X-MBX-APIKEY: " + getApiKey(argv[2]));
  LOG(INFO) << "Binance testnet GET /api/v3/account response received and saved to text file";
  const auto accountJson = nlohmann::json::parse(accountResponse);
  std::ofstream file(std::string(argv[1]) + "/account.txt");
  file << accountJson.dump(4);

  // Make Binance testnet order
  const std::string queryString =
      "symbol=BTCUSDT&side=BUY&type=LIMIT&timeInForce=GTC&quantity=0.0001&price=100000.00&recvWindow=5000&timestamp=" +
      std::to_string(time(nullptr) * 1000);
  const auto orderUrl = getOrderUrl(argv[2], queryString);
  std::string orderResponse = runHttpPostWithHeader(orderUrl, "X-MBX-APIKEY: " + getApiKey(argv[2]));
  const auto orderJson = nlohmann::json::parse(orderResponse);
  std::ofstream fileOrder(std::string(argv[1]) + "/order.txt");
  fileOrder << orderJson.dump(4);

  LOG(INFO) << "########## Ending AlgoTrader";
}
