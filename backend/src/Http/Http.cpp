#include "Http.hpp"

#include <curl/curl.h>
#include <spdlog/spdlog.h>

#include <cstdio>

namespace Http {

namespace {
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  const size_t contentSizeBytes = size * nmemb;
  static_cast<std::string*>(userp)->append(static_cast<char*>(contents), contentSizeBytes);
  return contentSizeBytes;
}
}  // namespace

std::string Http::get(const std::string& url, const std::string& header) const {
  CURL* curl = curl_easy_init();
  std::string response;

  if (curl) {
    struct curl_slist* headers = nullptr;
    if (header.size() > 0) {
      headers = curl_slist_append(headers, header.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    const auto res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      SPDLOG_ERROR("HTTP GET {} FAILED, error: {}", url, curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }
  SPDLOG_INFO("HTTP GET {} ; request header: {}; response body: {}", url, header, response);
  return response;
}

Response Http::post(const std::string& url, const std::string& header) const {
  CURL* curl = curl_easy_init();
  std::string responseBody;
  const std::string postFields = "";
  int64_t httpCode = 0;

  if (curl) {
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    const auto res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      SPDLOG_ERROR("HTTP POST {} FAILED, error:{}", url, curl_easy_strerror(res));
    } else {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }
  SPDLOG_INFO("HTTP POST {}; request header: {}; response code: {}; response body: {}", url, header, httpCode,
              responseBody);
  return {HttpStatusCode{httpCode}, HttpBody{responseBody}};
}

Response Http::del(const std::string& url, const std::string& header) const {
  CURL* curl = curl_easy_init();
  std::string responseBody;
  int64_t httpCode = 0;

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

    const auto res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      SPDLOG_ERROR("HTTP DELETE {} FAILED, error: {}", url, curl_easy_strerror(res));
    } else {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }
  SPDLOG_INFO("HTTP DELETE {}; request header: {}; response code: {}; response body: {}", url, header, httpCode,
              responseBody);
  return {HttpStatusCode{httpCode}, HttpBody{responseBody}};
}

}  // namespace Http