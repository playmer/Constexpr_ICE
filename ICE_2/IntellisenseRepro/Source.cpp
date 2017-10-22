#include "YTE/Meta/Type.hpp"

namespace YTE
{
  class Animal : public YTE::Base
  {
  public:
    DeclareType(Animal);
  };

  DefineType(Animal)
  {
    YTERegisterType(Animal);
    Animal::GetStaticType()->AddAttribute<TestAttribute>();
  }
}


//class Animal2 : public YTE::Base
//{
//public:
//  DeclareType(Animal2);
//};


int main()
{
  YTE::InitializeType<YTE::Animal>();
}