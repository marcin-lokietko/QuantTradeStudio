#include <glog/logging.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include <fstream>
#include <iomanip>
#include <sstream>

#include "Encryption.hpp"

namespace MarketService::Binance {
std::string Encryption::generateSignature(const std::string& data) const {
  const auto secretKey = getSecretKey();
  unsigned char* digest;
  digest = HMAC(EVP_sha256(), secretKey.c_str(), secretKey.size(), reinterpret_cast<const unsigned char*>(data.c_str()),
                data.size(), nullptr, nullptr);

  std::ostringstream result;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
  }
  return result.str();
}

std::string Encryption::getApiKey() const {
  std::ifstream stream(keysDir_.val_.string() + "/apiKey.txt");
  std::string key;
  stream >> key;
  return key;
}

std::string Encryption::getSecretKey() const {
  std::ifstream stream(keysDir_.val_.string() + "/secretKey.txt");
  std::string key;
  stream >> key;
  return key;
}

}  // namespace MarketService::Binance