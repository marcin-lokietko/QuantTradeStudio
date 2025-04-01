#pragma once

// A unique tag is needed to differentiate two separate wrappers of the same primitive.
template <typename T, typename UniqueTag>
struct StrongType {
  T val_;
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
