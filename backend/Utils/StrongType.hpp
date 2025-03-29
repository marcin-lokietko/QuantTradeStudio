#pragma once

// A unique tag is needed to differentiate two separate wrappers of the same primitive.
template <typename T, typename UniqueTag>
struct StrongType {
  T val_;
};

/*
Note that for e.g.
namespace A {
  DEFINE_STRONG_TYPE(Asset, std::string);
}
namespace B {
  DEFINE_STRONG_TYPE(Asset, std::string);
}
the resulting types are:
StrongType<std::string, A::AssetTag>
and
StrongType<std::string, B::AssetTag>

So the A::Asset and B::Asset are two separate types.
*/
#define DEFINE_STRONG_TYPE(Name, Type) using Name = StrongType<Type, struct Name##Tag>
