#include "Utils/StrongType.hpp"

#include <set>
#include <string>

#include "gmock/gmock.h"

DEFINE_STRONG_TYPE(IntType, int);
DEFINE_STRONG_TYPE(IntTypeAddable, int, Addable);

TEST(StrongTypeTest, objectsCanBeComparedEqual) {
  IntType a{10};
  IntType b{10};
  EXPECT_TRUE(a == b);
}

TEST(StrongTypeTest, objectsCanBeComparedNotEqual) {
  IntType a{5};
  IntType b{10};
  EXPECT_TRUE(a != b);
}

TEST(StrongTypeTest, objectsCanBeComparedSmaller) {
  IntType a{5};
  IntType b{10};
  EXPECT_TRUE(a < b);
}

TEST(StrongTypeTest, objectCanBeUsedAsStdContainerKey) {
  IntType a{5};
  std::set<IntType> mySet;
  mySet.insert(a);
  EXPECT_TRUE(mySet.find(a) != mySet.end());
}

TEST(StrongTypeTest, objectsOfAddableTypeCanBeAdded) {
  IntTypeAddable a{5};
  IntTypeAddable b{10};
  auto c = a + b;

  IntTypeAddable expectedSum{15};
  EXPECT_EQ(expectedSum, c);
  EXPECT_EQ(15, c.val_);
}
