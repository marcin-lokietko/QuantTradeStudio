#include <curl/curl.h>
#include <glog/logging.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "BinanceService.h"

namespace StockMarketService {

const std::string binanceTestnetBaseUrl = "https://testnet.binance.vision";
const std::string binanceRealBaseUrl = "https://api.binance.com";

size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  const size_t contentSizeBytes = size * nmemb;
  static_cast<std::string*>(userp)->append(static_cast<char*>(contents), contentSizeBytes);
  return contentSizeBytes;
}

std::string runHttpGet(const std::string& url) {
  CURL* curl = curl_easy_init();
  std::string response;

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
    const auto res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      LOG(ERROR) << "Failed, error: " << curl_easy_strerror(res);
    }

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

std::string getAccountUrl(const std::string& secretKey) {
  const std::string timestamp = "timestamp=" + std::to_string(time(nullptr) * 1000);
  const std::string signature = generateSignature(timestamp, secretKey);
  const std::string signedQuery = timestamp + "&signature=" + signature;
  return binanceTestnetBaseUrl + "/api/v3/account?" + signedQuery;
}

std::string getOrderUrl(const std::string& secretKey, const std::string& queryString) {
  const std::string signature = generateSignature(queryString, secretKey);
  const std::string signedQuery = queryString + "&signature=" + signature;
  return binanceTestnetBaseUrl + "/api/v3/order?" + signedQuery;
}

std::string BinanceService::getServerTime() {
  const std::string timeUrl = binanceTestnetBaseUrl + "/api/v3/time";
  return runHttpGet(timeUrl);
}

std::string BinanceService::getPrice(const std::string& symbol) {
  std::string url = binanceTestnetBaseUrl + "/api/v3/ticker/price?symbol=" + symbol;
  const auto response = runHttpGet(url);

  const auto jsonResponse = nlohmann::json::parse(response);
  if (jsonResponse.contains("price")) {
    return jsonResponse["price"];
  } else {
    LOG(ERROR) << "Failed to retrieve price.";
  }
  return "";
}

std::string BinanceService::getKlines(const std::string& symbol, const std::string& interval) {
  const std::string klinesUrl =
      binanceTestnetBaseUrl + "/api/v3/klines?symbol=" + symbol + "&interval=" + interval + "&limit=1000";
  return runHttpGet(klinesUrl);
}

std::string BinanceService::getAccountData() {
  const auto accountUrl = getAccountUrl(getSecretKey(keysPath));
  return runHttpGetWithHeader(accountUrl, "X-MBX-APIKEY: " + getApiKey(keysPath));
}

void BinanceService::makeOrder() {
  const std::string queryString =
      "symbol=BTCUSDT&side=BUY&type=LIMIT&timeInForce=GTC&quantity=0.0001&price=100000.00&recvWindow=5000&timestamp=" +
      std::to_string(time(nullptr) * 1000);
  const auto orderUrl = getOrderUrl(getSecretKey(keysPath), queryString);
  const auto response = runHttpPostWithHeader(orderUrl, "X-MBX-APIKEY: " + getApiKey(keysPath));

  LOG(INFO) << "Order complete, response: " << response;
}

}  // namespace StockMarketService
