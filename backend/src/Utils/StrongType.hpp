#pragma once

#include <format>
#include <optional>
#include <type_traits>
#include <utility>

#include "Utils/ToString.hpp"

struct Addable {};

// A unique tag is needed to differentiate two separate wrappers of the same primitive.
template <typename T, typename UniqueTag, typename... Mixins>
struct StrongType : public Mixins... {
  using WrapperType = StrongType<T, UniqueTag, Mixins...>;
  T val_;

  constexpr StrongType() = default;
  explicit constexpr StrongType(T val) : val_(std::move(val)) {}

  bool operator<(const WrapperType& other) const { return val_ < other.val_; }
  bool operator>=(const WrapperType& other) const { return val_ >= other.val_; }
  bool operator==(const WrapperType& other) const { return val_ == other.val_; }
  bool operator!=(const WrapperType& other) const { return val_ != other.val_; }

  WrapperType operator+(const WrapperType& other)
    requires std::is_base_of_v<Addable, WrapperType>
  {
    return WrapperType{val_ + other.val_};
  }
};

// Those functions enable std::format (and thus spdlog with std::format backend) support for StrongType and
// std::optional<StrongType> by just delegating to ::toString functions (or formarring the underlying type directly if
// it is arithmetic).
template <typename T, typename UniqueTag, typename... Mixins>
struct std::formatter<StrongType<T, UniqueTag, Mixins...>> : std::formatter<T> {
  template <typename FormatContext>
  auto format(const StrongType<T, UniqueTag, Mixins...>& val, FormatContext& ctx) const {
    if constexpr (std::is_arithmetic_v<T>) {
      return std::formatter<T>::format(val.val_, ctx);
    } else {
      return std::formatter<std::string>::format(::toString(val), ctx);
    }
  }
};

template <typename T, typename UniqueTag, typename... Mixins>
struct std::formatter<std::optional<StrongType<T, UniqueTag, Mixins...>>> : std::formatter<std::string> {
  template <typename FormatContext>
  auto format(const std::optional<StrongType<T, UniqueTag, Mixins...>>& val, FormatContext& ctx) const {
    return std::formatter<std::string>::format(::toString(val), ctx);
  }
};

/*
Usage:
DEFINE_STRONG_TYPE(AssetSymbol, std::string);

----------------------------------------
Note that for e.g.
namespace A {
  DEFINE_STRONG_TYPE(AssetSymbol, std::string);
}
namespace B {
  DEFINE_STRONG_TYPE(AssetSymbol, std::string);
}
the resulting types are:
StrongType<std::string, A::AssetSymbolTag>
and
StrongType<std::string, B::AssetSymbolTag>

So the A::AssetSymbol and B::AssetSymbol are two separate types.
*/
#define DEFINE_STRONG_TYPE(Name, Type, ...) using Name = StrongType<Type, struct Name##Tag, ##__VA_ARGS__>
