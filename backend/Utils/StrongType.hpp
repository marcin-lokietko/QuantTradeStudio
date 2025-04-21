#pragma once

struct Addable {};

// A unique tag is needed to differentiate two separate wrappers of the same primitive.
template <typename T, typename UniqueTag, typename... Mixins>
struct StrongType : public Mixins... {
  using WrapperType = StrongType<T, UniqueTag, Mixins...>;
  T val_;

  bool operator<(const WrapperType& other) const { return val_ < other.val_; }
  bool operator==(const WrapperType& other) const = default;
  bool operator!=(const WrapperType& other) const = default;

  WrapperType operator+(const WrapperType& other)
    requires std::is_base_of_v<Addable, WrapperType>
  {
    return WrapperType{val_ + other.val_};
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
StrongType<std::string, A::AssetTag>
and
StrongType<std::string, B::AssetTag>

So the A::AssetSymbol and B::AssetSymbol are two separate types.
*/
#define DEFINE_STRONG_TYPE(Name, Type) using Name = StrongType<Type, struct Name##Tag>
