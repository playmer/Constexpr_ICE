#include "YTE/Meta/Type.hpp"

namespace YTE
{
  std::unordered_map<std::string, Type*> Type::sGlobalTypes;

  DefineType(DocumentedObject)
  {
    YTERegisterType(DocumentedObject);
    YTEBindProperty(&DocumentedObject::GetDocumentation, &DocumentedObject::SetDocumentation, "Documentation");
  }

  DefineType(Type)
  {
    YTERegisterType(Type);

  }

  DefineType(Function)
  {
    YTERegisterType(Function);

  }


  DefineType(Property)
  {
    YTERegisterType(Property);

  }


  DefineType(Field)
  {
    YTERegisterType(Field);
  }
}

DefineExternalType(YTE::s8)
{
  YTERegisterType(YTE::s8);
}

DefineExternalType(YTE::i8)
{
  YTERegisterType(YTE::i8);
}

DefineExternalType(YTE::i16)
{
  YTERegisterType(YTE::i16);
}

DefineExternalType(YTE::i32)
{
  YTERegisterType(YTE::i32);
}

DefineExternalType(YTE::i64)
{
  YTERegisterType(YTE::i64);
}

DefineExternalType(YTE::u8)
{
  YTERegisterType(YTE::u8);
}

DefineExternalType(YTE::u16)
{
  YTERegisterType(YTE::u16);
}

DefineExternalType(YTE::u32)
{
  YTERegisterType(YTE::u32);
}

DefineExternalType(YTE::u64)
{
  YTERegisterType(YTE::u64);
}

DefineExternalType(void)
{
  YTERegisterType(void);
}

DefineExternalType(bool)
{
  YTERegisterType(bool);
}

DefineExternalType(float)
{
  YTERegisterType(float);
}

DefineExternalType(double)
{
  YTERegisterType(double);
}

DefineExternalType(std::string)
{
  YTERegisterType(std::string);
}