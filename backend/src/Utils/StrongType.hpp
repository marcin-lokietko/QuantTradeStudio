#pragma once

#include <fmt/format.h>

#include <optional>
#include <type_traits>
#include <utility>

struct Addable {};

// A unique tag is needed to differentiate two separate wrappers of the same primitive.
template <typename T, typename UniqueTag, typename... Mixins>
struct StrongType : public Mixins... {
  using WrapperType = StrongType<T, UniqueTag, Mixins...>;
  T val_;

  constexpr StrongType() = default;
  explicit constexpr StrongType(T val) : val_(std::move(val)) {}

  bool operator<(const WrapperType& other) const { return val_ < other.val_; }
  bool operator==(const WrapperType& other) const { return val_ == other.val_; }
  bool operator!=(const WrapperType& other) const { return val_ != other.val_; }

  WrapperType operator+(const WrapperType& other)
    requires std::is_base_of_v<Addable, WrapperType>
  {
    return WrapperType{val_ + other.val_};
  }
};

// Allows to make fmt::format (used by spdlog) to work with StrongType:
template <typename T, typename UniqueTag, typename... Mixins>
struct fmt::formatter<StrongType<T, UniqueTag, Mixins...>> : fmt::formatter<T> {
  template <typename FormatContext>
  auto format(const StrongType<T, UniqueTag, Mixins...>& val, FormatContext& ctx) const {
    return fmt::formatter<T>::format(val.val_, ctx);
  }
};

template <typename T, typename UniqueTag, typename... Mixins>
struct fmt::formatter<std::optional<StrongType<T, UniqueTag, Mixins...>>> : fmt::formatter<std::string> {
  template <typename FormatContext>
  auto format(const std::optional<StrongType<T, UniqueTag, Mixins...>>& opt, FormatContext& ctx) const {
    if (opt.has_value()) {
      return fmt::formatter<std::string>::format(fmt::format("{}", opt.value()), ctx);
    } else {
      return fmt::formatter<std::string>::format("std::nullptr", ctx);
    }
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
