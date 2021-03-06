#pragma once
#include "YTE/Meta/Type.hpp"

namespace YTE
{
  class Component;

  class Attribute : public Base
  {
  public:
    DeclareType(Attribute)
  };

  class EditorProperty : public Attribute
  {
  public:
    DeclareType(EditorProperty);
    EditorProperty(DocumentedObject *aObject, bool aVisible = true);

    bool mVisible;
  };


  class TestAttribute : public Attribute
  {
  public:
    DeclareType(TestAttribute);
    TestAttribute(DocumentedObject *aObject);
  };

  typedef std::vector <std::string> (*StrGettor)(Component*);

  class DropDownStrings : public Attribute
  {
  public:
    DeclareType(DropDownStrings);
    DropDownStrings(DocumentedObject *aObject, StrGettor aStrGettor);

    StrGettor GetStringGettor();

  private:
    StrGettor mStringGettor;
  };
}