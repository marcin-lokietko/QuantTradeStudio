#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

template <typename T, typename UniqueTag, typename... Mixins>
class StrongType;

template <typename T, typename UniqueTag, typename... Mixins>
inline std::string toString(const StrongType<T, UniqueTag, Mixins...>& strongTypedObject) {
  return std::format("{}", strongTypedObject.val_);
}

template <typename T>
concept IsArgOfToStringMethod = requires(const T& t) {
  { toString(t) } -> std::convertible_to<std::string>;
};

template <IsArgOfToStringMethod T>
inline std::string toString(const std::vector<T>& container) {
  std::string ret{};
  for (const auto& elem : container) {
    ret += toString(elem);
    ret += ",";
  }
  if (ret.size() > 0) {
    ret.pop_back();
  }
  return "[" + ret + "]";
}

/* Note that
template <IsArgOfToStringMethod T>
inline std::string toString(const std::optional<T>& opt)

Would not be enough, because it does not support
std::optional<std::vector<SomeArgOfStringMethod>>
because for some reason the
template <IsArgOfToStringMethod T> inline std::string toString(const std::vector<T>& container)
is not considered for "std::vector<SomeArgOfStringMethod>" is this case.

Also, support for nullopt vector is needed, because empty vector is not necessarily the same as nullopt vector - in this
project nullopt sometimes means failed deserialization and empty vector is a meaningful message.
*/
template <typename T>
concept IsOptionalOfArgOfToStringMethod = requires(const std::optional<T>& opt) {
  { toString(opt.value()) } -> std::convertible_to<std::string>;
};

template <IsOptionalOfArgOfToStringMethod T>
inline std::string toString(const std::optional<T>& optional) {
  if (!optional.has_value()) {
    return "std::nullopt";
  }
  return toString(optional.value());
}

inline std::string toString(std::time_t time) {
  const std::tm localTm = *std::localtime(&time);
  std::ostringstream oss;
  oss << std::put_time(&localTm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

inline std::string toString(const std::chrono::system_clock::time_point& tp) {
  const std::time_t time = std::chrono::system_clock::to_time_t(tp);
  return toString(time);
}