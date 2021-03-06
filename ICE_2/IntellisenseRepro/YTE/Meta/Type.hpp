#pragma once

#ifndef YTE_Meta_Type_hpp
#define YTE_Meta_Type_hpp

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "YTE/StandardLibrary/OrderedMultiMap.hpp"
#include "YTE/StandardLibrary/OrderedMap.hpp"
#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Meta/ForwardDeclarations.hpp"
#include "YTE/Meta/Reflection.hpp"

#define DebugObjection(a1, a2, ...) 

namespace YTE
{
  class Type;

  class Base
  {
  public:
    virtual Type *GetType() = 0;
    virtual ~Base() {};
  };



  template <typename T>
  typename T::SelfType GetSelfType(typename T::SelfType*) {}

  template <typename T>
  void GetSelfType(...) {}


  template <typename T>
  T GetDummy(void (T::*)());



  // Used to declare a static type within a class
  // Requires DefineType be used at some point in a
  // translation unit.
#define DeclareType(Name)                                      \
void Dummy() {}                                                \
typedef decltype(YTE::GetDummy(&Name::Dummy)) TempSelfType;         \
typedef decltype(YTE::GetSelfType<TempSelfType>(nullptr)) BaseType; \
typedef TempSelfType SelfType;                                 \
static Type sType;                                             \
static Type* GetStaticType() { return &sType; };               \
Type* GetType() { return &sType; };                            \
static void InitializeType();


#define DefineType(Name)                                 \
Type Name::sType{#Name,                                  \
                 static_cast<Name*>(nullptr),            \
                 static_cast<Name::BaseType*>(nullptr)}; \
void Name::InitializeType()

  enum class PropertyBinding
  {
    Get,
    Set,
    GetSet
  };


#define YTEParameterNames(...) {__VA_ARGS__}
#define YTENoNames {}
#define YTENoOverload
#define YTENoSetter nullptr
#define YTENoGetter nullptr

#define YTERegisterType(aType) Type::AddGlobalType(TypeId<aType>()->GetName(), TypeId<aType>())

#define YTEAddFunction(aFunctionPointer, aOverloadResolution, aFunctionName, aInitializerListOfNames)  \
  BindFunction<decltype(aOverloadResolution aFunctionPointer),                                         \
               aFunctionPointer,                                                                       \
               std::initializer_list<const char*>aInitializerListOfNames.size()>(                      \
    aFunctionName,                                                                                     \
    TypeId<DecomposeFunctionObjectType<decltype(aOverloadResolution aFunctionPointer)>::ObjectType>(), \
    aInitializerListOfNames)


#define YTEBindEnum(aType) ;

#define YTEBindField(aFieldPointer, aName, aPropertyBinding)              \
  BindField<decltype(aFieldPointer), aFieldPointer>(                      \
    aName,                                                                \
    aPropertyBinding,                                                     \
    TypeId<DecomposeFieldPointer<decltype(aFieldPointer)>::ObjectType>())

#define YTEBindProperty(aGetterFunction, aSetterFunction, aName)            \
  BindProperty<decltype(aGetterFunction), aGetterFunction,                  \
               decltype(aSetterFunction), aSetterFunction>(                 \
    aName,                                                                  \
    TypeId<DecomposePropertyType<decltype(aGetterFunction),                 \
                                 decltype(aSetterFunction)>::ObjectType>())

  class DocumentedObject : public Base
  {
  public:
    DeclareType(DocumentedObject);

    DocumentedObject(const DocumentedObject &) = delete;

    DocumentedObject()
      : mDocumentation("")
    {

    }


    DocumentedObject(const char *aDocumentation)
      : mDocumentation(aDocumentation)
    {

    }

    std::string GetDocumentation()
    {
      return mDocumentation;
    }

    void SetDocumentation(const char *aString)
    {
      mDocumentation = aString;
    }

    std::string& Description()
    {
      return mDocumentation;
    }

    template <typename tType, typename... tArguments>
    DocumentedObject& AddAttribute(tArguments &&...aArguments);


    template <typename tType>
    tType* GetAttribute();

  private:
    OrderedMap<Type*, std::unique_ptr<Attribute>> mAttributes;
    std::string mDocumentation;
  };

  class Type : public DocumentedObject
  {
  public:
    DeclareType(Type)

    using DefaultConstructor = void(*)(void*);
    using CopyConstructor = void(*)(const void*, void*);
    using MoveConstructor = void(*)(void*, void*);
    using Destructor = void(*)(void*);

    enum class Modifier
    {
      Normal,
      Reference,
      Pointer,
      Const
    };

    template <typename tType>
    explicit Type(const char *aName, tType *aNull);

    template <typename tDerived, typename tBase>
    explicit Type(const char *aName, tDerived *aDerivedNull, tBase *aBaseNull);


    template <typename tDerived, typename tBase>
    explicit Type(tDerived *aDerivedNull, tBase *aBaseNull);

    template <typename T>
    explicit Type(Type *aType, Modifier aModifier, T *aNull);

    template <typename T>
    explicit Type(Type *aType, Modifier aModifier, T *aNull, bool aFalse);

    Type(Type&) = delete;

    ~Type();

    const std::string& Name()  const { return mName; }
    size_t             Hash() const { return mHash; }
    size_t             GetAllocatedSize() const { return mAllocatedSize; }
    size_t             GetStoredSize() const { return mStoredSize; }
    size_t             GetUnqualifiedSize() const { return mUnqualifiedSize; }
    DefaultConstructor GetDefaultConstructor() const { return mDefaultConstructor; }
    CopyConstructor    GetCopyConstructor()  const { return mCopyConstructor; }
    MoveConstructor    GetMoveConstructor()  const { return mMoveConstructor; }
    Destructor         GetDestructor()  const { return mDestructor; }

    // Don't really need to do this, but w/e.
    bool operator==(const Type &aRight) const
    {
      if ((mHash == aRight.mHash) && (mName == aRight.mName))
      {
        return true;
      }

      return false;
    }

    void AddFunction(std::unique_ptr<Function> aFunction);
    void AddProperty(std::unique_ptr<Property> aProperty);
    void AddField(std::unique_ptr<Field>    aField);

    OrderedMultiMap<std::string, std::unique_ptr<Function>>::range GetFunctionRange(const char *aName)
    {
      std::string name{ aName };

      return mFunctions.FindAll(name);
    }

    Function* GetFirstFunction(const char *aName)
    {
      std::string name{ aName };

      auto it = mFunctions.FindFirst(name);

      if (it != mFunctions.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>::range GetPropertyRange(const char *aName)
    {
      std::string name{ aName };

      return mProperties.FindAll(name);
    }

    Property* GetFirstProperty(const std::string &aName)
    {
      auto it = mProperties.FindFirst(aName);

      if (it != mProperties.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    Property* GetFirstProperty(const char *aName)
    {
      std::string name{ aName };

      auto it = mProperties.FindFirst(name);

      if (it != mProperties.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    Property* GetFirstField(const std::string &aName)
    {
      auto it = mFields.FindFirst(aName);

      if (it != mFields.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>::range GetFieldRange(const std::string &aName)
    {
      return mFields.FindAll(aName);
    }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>::range GetFieldRange(const char *aName)
    {
      std::string name{ aName };

      return mFields.FindAll(name);
    }

    bool IsA(Type *aType)
    {
      Type *base = this;

      if (base == aType)
      {
        return true;
      }

      while (base->GetBaseType())
      {
        base = base->GetBaseType();

        if (base == aType)
        {
          return true;
        }
      }

      return false;
    }

    template<typename tType>
    bool IsA();

    Type* GetMostBasicType()
    {
      Type *type = this;

      while (type->GetPointerTo() ||
             type->GetReferenceTo() ||
             type->GetConstOf())
      {
        if (type->GetPointerTo())
        {
          type = type->GetPointerTo();
        }
        else if (type->GetReferenceTo())
        {
          type = type->GetReferenceTo();
        }
        else if (type->GetConstOf())
        {
          type = type->GetConstOf();
        }
      }

      return type;
    }
    Property* GetFirstField(const char *aName)
    {
      std::string name{ aName };

      auto it = mFields.FindFirst(name);

      if (it != mFields.end())
      {
        return it->second.get();
      }

      return nullptr;
    }

    Type* GetPointerTo()
    {
      return mPointerTo;
    }

    Type* GetReferenceTo()
    {
      return mReferenceTo;
    }

    Type* GetConstOf()
    {
      return mConstOf;
    }

    Type* GetEnumOf()
    {
      return mEnumOf;
    }

    void SetEnumOf(Type *aType)
    {
      mEnumOf = aType;
    }


    Type* GetBaseType()
    {
      return mBaseType;
    }

    const std::string& GetName() const
    {
      return mName;
    }

    static void AddGlobalType(const std::string &aName, Type *aType)
    {
      //auto it = sGlobalTypes.find(aName);
      //
      //if (it != sGlobalTypes.end())
      //{
      //  std::cout << "Type of the name " << aName << " already exists, not adding." << std::endl;
      //  return;
      //}
      //
      //sGlobalTypes.emplace(aName, aType);
    }

    static Type* GetGlobalType(const std::string &aName)
    {
      //auto it = sGlobalTypes.find(aName);
      //
      //Type *toReturn{ nullptr };
      //
      //if (it != sGlobalTypes.end())
      //{
      //  toReturn = it->second;
      //}
      //
      //if (toReturn == nullptr)
      //{
      //  printf("Could not find a type named %s, did you rename/misspell it/forget to Define/InitializeType it?", aName.c_str());
      //}
      //
      //return toReturn;
    }

    OrderedMultiMap<std::string, std::unique_ptr<Property>>& GetProperties() { return mProperties; };
    OrderedMultiMap<std::string, std::unique_ptr<Property>>& GetFields() { return mFields; };

  private:
    OrderedMultiMap<std::string, std::unique_ptr<Function>> mFunctions;
    OrderedMultiMap<std::string, std::unique_ptr<Property>> mProperties;
    OrderedMultiMap<std::string, std::unique_ptr<Property>> mFields;
    std::string mName;
    size_t mHash;
    size_t mAllocatedSize;
    size_t mStoredSize;
    size_t mUnqualifiedSize;
    DefaultConstructor mDefaultConstructor;
    CopyConstructor mCopyConstructor;
    MoveConstructor mMoveConstructor;
    Destructor mDestructor;

    Type *mReferenceTo;
    Type *mPointerTo;
    Type *mConstOf;
    Type *mBaseType;
    Type *mEnumOf;

    static std::unordered_map<std::string, Type*> sGlobalTypes;
  };


  template<typename T>
  inline Type* TypeId();


  template<typename T>
  inline void InitializeType();

  template<typename T>
  struct TypeIdentification
  {
    static inline Type* TypeId()
    {
      return T::GetStaticType();
    }

    static inline void InitializeType()
    {
      T::InitializeType();
    }
  };

  template<typename T>
  struct TypeIdentification<T*>
  {
    static inline Type* TypeId()
    {
      // TODO (Austin): Check to see if this needs the bool passed in at the end of the constructor.
      static Type type{ ::YTE::TypeId<T>(), Type::Modifier::Pointer, static_cast<T**>(nullptr), false };

      return &type;
    }
  };

  template<typename T>
  struct TypeIdentification<T&>
  {
    static inline Type* TypeId()
    {
      static Type type{ ::YTE::TypeId<T>(), Type::Modifier::Reference, static_cast<T*>(nullptr) };

      return &type;
    }
  };


  template<typename T>
  struct TypeIdentification<const T>
  {
    static inline Type* TypeId()
    {
      static Type type{ ::YTE::TypeId<T>(), Type::Modifier::Const, static_cast<const T*>(nullptr) };

      return &type;
    }
  };


  template<typename T>
  inline Type* TypeId()
  {
    return TypeIdentification<T>::TypeId();
  }

  template<typename T>
  inline void InitializeType()
  {
    TypeIdentification<T>::InitializeType();
  }
}

#define DeclareExternalType(Name)                          \
namespace YTE                                              \
{                                                          \
  template<>                                               \
  struct TypeIdentification<Name>                          \
  {                                                        \
    static inline Type* TypeId()                           \
    {                                                      \
      static Type type{ #Name,                             \
                        static_cast<Name*>(nullptr) };     \
      return &type;                                        \
    }                                                      \
                                                           \
    static inline void InitializeType()                    \
    {                                                      \
      Type::AddGlobalType(TypeId()->GetName(), TypeId());  \
    }                                                      \
  };                                                       \
                                                           \
  template<>                                               \
  void InitializeType<Name>();                             \
}


  #define DefineExternalType(aName) template<> void YTE::InitializeType<aName>()


  DeclareExternalType(YTE::s8)
  DeclareExternalType(YTE::i8)
  DeclareExternalType(YTE::i16)
  DeclareExternalType(YTE::i32)
  DeclareExternalType(YTE::i64)
  DeclareExternalType(YTE::u8)
  DeclareExternalType(YTE::u16)
  DeclareExternalType(YTE::u32)
  DeclareExternalType(YTE::u64)
  DeclareExternalType(void)
  DeclareExternalType(bool)
  DeclareExternalType(float)
  DeclareExternalType(double)
  DeclareExternalType(std::string)

#include "YTE/Meta/Function.hpp"
#include "YTE/Meta/Property.hpp"
#include "YTE/Meta/Field.hpp"
#include "YTE/Meta/Attribute.hpp"

namespace YTE
{
  template <typename tType, typename... tArguments>
  DocumentedObject& DocumentedObject::AddAttribute(tArguments &&...aArguments)
  {
    mAttributes.Emplace(TypeId<tType>(), std::make_unique<tType>(this, std::forward<tArguments &&>(aArguments)...));

    return (*this);
  }

  template <typename tType>
  tType* DocumentedObject::GetAttribute()
  {
    auto it = mAttributes.Find(TypeId<tType>());
    tType *toReturn = nullptr;

    if (it != mAttributes.end())
    {
      toReturn = static_cast<tType*>(it->second.get());
    }

    return toReturn;
  }


  template <typename tDerived, typename tBase>
  inline Type::Type(const char *, tDerived *, tBase *)
    : mName(),
    mHash(std::hash<std::string>{}(mName)),
    mAllocatedSize(SizeOf<tDerived>()),
    mStoredSize(SizeOf<tDerived>()),
    mUnqualifiedSize(SizeOf<typename StripQualifiers<tDerived>::type>()),
    mDefaultConstructor(GenericDefaultConstruct<typename StripSingleQualifier<tDerived>::type>),
    mCopyConstructor(GenericCopyConstruct<typename StripSingleQualifier<tDerived>::type>),
    mMoveConstructor(GenericMoveConstruct<typename StripSingleQualifier<tDerived>::type>),
    mDestructor(GenericDestruct<tDerived>),
    mReferenceTo(nullptr),
    mPointerTo(nullptr),
    mConstOf(nullptr),
    mBaseType(TypeId<tBase>())
  {
    constexpr auto name = GetTypeName<tDerived>();
    mName = name.data();
  }

  template <typename tType>
  inline Type::Type(const char *, tType *)
    : mName(),
    mHash(std::hash<std::string>{}(mName)),
    mAllocatedSize(SizeOf<tType>()),
    mStoredSize(SizeOf<tType>()),
    mUnqualifiedSize(SizeOf<typename StripQualifiers<tType>::type>()),
    mDefaultConstructor(GenericDefaultConstruct<typename StripSingleQualifier<tType>::type>),
    mCopyConstructor(GenericCopyConstruct<typename StripSingleQualifier<tType>::type>),
    mMoveConstructor(GenericMoveConstruct<typename StripSingleQualifier<tType>::type>),
    mDestructor(GenericDestruct<tType>),
    mReferenceTo(nullptr),
    mPointerTo(nullptr),
    mConstOf(nullptr),
    mBaseType(nullptr)
  {
    constexpr auto name = GetTypeName<tType>();
    mName = name.data();
  }


  template <typename tDerived, typename tBase>
  inline Type::Type(tDerived *, tBase *)
    : mName(),
    mHash(std::hash<std::string>{}(mName)),
    mAllocatedSize(SizeOf<tDerived>()),
    mStoredSize(SizeOf<tDerived>()),
    mUnqualifiedSize(SizeOf<StripQualifiers<tDerived>::type>()),
    mDefaultConstructor(GenericDefaultConstruct<typename StripSingleQualifier<tDerived>::type>),
    mCopyConstructor(GenericCopyConstruct<typename StripSingleQualifier<tDerived>::type>),
    mMoveConstructor(GenericMoveConstruct<typename StripSingleQualifier<tDerived>::type>),
    mDestructor(GenericDestruct<tDerived>),
    mReferenceTo(nullptr),
    mPointerTo(nullptr),
    mConstOf(nullptr),
    mBaseType(TypeId<tBase>())
  {
    constexpr auto name = GetTypeName<tDerived>();
    mName = name.data();
  }




  template <typename T>
  inline Type::Type(Type *aType, Modifier aModifier, T *)
    : mName(),
    mHash(std::hash<std::string>{}(mName)),
    mAllocatedSize(SizeOf<T>()),
    mStoredSize(SizeOf<T>()),
    mUnqualifiedSize(SizeOf<typename StripQualifiers<T>::type>()),
    mDefaultConstructor(GenericDefaultConstruct<typename StripSingleQualifier<T>::type>),
    mCopyConstructor(GenericCopyConstruct<typename StripSingleQualifier<T>::type>),
    mMoveConstructor(GenericMoveConstruct<typename StripSingleQualifier<T>::type>),
    mDestructor(GenericDestruct<T>),
    mReferenceTo(nullptr),
    mPointerTo(nullptr),
    mConstOf(nullptr),
    mBaseType(nullptr)
  {
    constexpr auto name = GetTypeName<T>();
    mName = name.data();

    switch (aModifier)
    {
    case Modifier::Const:
    {
      mConstOf = aType;
      break;
    }
    case Modifier::Reference:
    {
      mReferenceTo = aType;
      break;
    }
    case Modifier::Pointer:
    {
      mPointerTo = aType;
      break;
    }
    case Modifier::Normal:
    default:
      break;
    }
  }

  template <typename T>
  inline Type::Type(Type *aType, Modifier aModifier, T*, bool)
    : mName(),
    mHash(std::hash<std::string>{}(mName)),
    mAllocatedSize(SizeOf<T*>()),
    mStoredSize(SizeOf<T*>()),
    mUnqualifiedSize(SizeOf<typename StripQualifiers<T *>::type>()),
    mDefaultConstructor(GenericDefaultConstruct<T*>),
    mCopyConstructor(GenericCopyConstruct<T*>),
    mMoveConstructor(GenericMoveConstruct<T*>),
    mDestructor(GenericDestruct<T*>),
    mReferenceTo(nullptr),
    mPointerTo(nullptr),
    mConstOf(nullptr),
    mBaseType(nullptr)
  {
    constexpr auto name = GetTypeName<T>();
    mName = name.data();

    switch (aModifier)
    {
    case Modifier::Const:
    {
      mConstOf = aType;
      break;
    }
    case Modifier::Reference:
    {
      mReferenceTo = aType;
      break;
    }
    case Modifier::Pointer:
    {
      mPointerTo = aType;
      break;
    }
    case Modifier::Normal:
    default:
      break;
    }
  }

  inline Type::~Type()
  {

  }

  inline void Type::AddFunction(std::unique_ptr<Function> aFunction)
  {
    mFunctions.Emplace(aFunction->GetName(), std::move(aFunction));
  }

  inline void Type::AddProperty(std::unique_ptr<Property> aProperty)
  {
    mProperties.Emplace(aProperty->GetName(), std::move(aProperty));
  }

  inline void Type::AddField(std::unique_ptr<Field> aField)
  {
    mFields.Emplace(aField->GetName(), std::move(aField));
  }


  template<typename tType>
  inline bool Type::IsA()
  {
    return IsA(TypeId<tType>());
  }
}

#endif
