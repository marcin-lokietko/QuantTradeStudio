#include <curl/curl.h>
#include <glog/logging.h>

#include <cstdio>

#include "Http.h"

namespace Http {

namespace {
size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  const size_t contentSizeBytes = size * nmemb;
  static_cast<std::string*>(userp)->append(static_cast<char*>(contents), contentSizeBytes);
  return contentSizeBytes;
}
}  // namespace

std::string Http::get(const std::string& url, const std::string& header) {
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
      LOG(ERROR) << "HTTP GET failed, error: " << curl_easy_strerror(res);
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }
  return response;
}

std::string Http::post(const std::string& url, const std::string& header) {
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

}  // namespace Http