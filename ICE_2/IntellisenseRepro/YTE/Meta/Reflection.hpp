#pragma once

#include "YTE/StandardLibrary/ConstexprString.hpp"

#if defined(_MSC_VER) && !defined(__EDG__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define CONSTEXPR
#else
#define CONSTEXPR constexpr
#endif


namespace YTE
{
  inline CONSTEXPR size_t GetTypeStart(const char *aTypeNameString)
  {
    (void)aTypeNameString;
    size_t beginTrim = 0;

#if defined(__clang__) && defined(__GNUC__)
    beginTrim = 46;
#elif defined(_MSC_VER)
    if (*(aTypeNameString + 39) == 's') // Where class or struct appears in MSVC
    {
      beginTrim = 46; //Where typename begins appearing in MSVC if struct.
    }
    else
    {
      beginTrim = 45; //Where typename begins appearing in MSVC if class.
    }
#endif

    return beginTrim;
  }

  inline CONSTEXPR size_t GetTypeEnd()
  {
    size_t endTrim = 0;

#if defined(__clang__)
    endTrim = 1;
#elif defined(_MSC_VER)
    endTrim = 7;
#endif

    return endTrim;
  }

  //template <typename T, T aFunction>
  //CONSTEXPR auto GetFunctionSignature()
  //{
  //  CONSTEXPR const char* typeName = __PRETTY_FUNCTION__;
  //
  //  CONSTEXPR size_t required_length = StringLength(typeName);
  //  ConstexprToken<required_length> test{ typeName };
  //
  //  return test;
  //}

  inline CONSTEXPR bool IsWhiteSpace(char aCharacter)
  {
    if (((9 >= aCharacter) && (aCharacter <= 13)) || ' ' == aCharacter)
    {
      return true;
    }

    return false;
  }


  inline CONSTEXPR bool IsIdentifier(char aCharacter)
  {
    if ((('a' <= aCharacter) && (aCharacter <= 'z')) ||
      (('A' <= aCharacter) && (aCharacter <= 'Z')) ||
      (('0' <= aCharacter) && (aCharacter <= '9')) ||
      '_' == aCharacter)
    {
      return true;
    }

    return false;
  }

  inline CONSTEXPR StringRange GetToken(StringRange aRange)
  {
    auto i = aRange.mBegin;

    while (!IsWhiteSpace(*i) && IsIdentifier(*i) && i < aRange.mEnd)
    {
      ++i;
    }

    // Gotta check if it's actually not an identifier and continue moving.
    if (i == aRange.mBegin)
    {
      while (!IsWhiteSpace(*i) && !IsIdentifier(*i) && i < aRange.mEnd)
      {
        ++i;
      }
    }

    // And finally simply check for whitespace.
    if (i == aRange.mBegin)
    {
      while (IsWhiteSpace(*i) && i < aRange.mEnd)
      {
        ++i;
      }
    }

    aRange.mEnd = i;
    return aRange;
  }

  template <typename T>
  inline CONSTEXPR auto GetTypeName()
  {
    CONSTEXPR const char* functionName = __PRETTY_FUNCTION__;

    // TODO: Should also work for GCC.
#if defined(__clang__)
    CONSTEXPR size_t firstBracket = GetFirstInstanceOfCharacter(functionName, StringLength(functionName), '[') + 1;
    CONSTEXPR size_t lastBracket = GetLastInstanceOfCharacter(functionName, StringLength(functionName), ']');

    CONSTEXPR size_t typenameTotalRangeSize = lastBracket - firstBracket;

    ConstexprTokenWriter<typenameTotalRangeSize + 1> finalName;

    StringRange totalType{ functionName + firstBracket, functionName + lastBracket };

    size_t i = 0;

    while (totalType.mBegin < totalType.mEnd)
    {
      auto token = GetToken(totalType);

      if (i < 2)
      {
        ++token.mEnd;
      }
      else
      {
        finalName.Write(token);
      }

      totalType.mBegin = token.mEnd;
      ++i;
    }

    //auto finalName = functionName;

#elif defined(__GNUC__)
    CONSTEXPR size_t firstBracket = GetFirstInstanceOfCharacter(functionName, StringLength(functionName), '[') + 1;
    CONSTEXPR size_t lastBracket = GetLastInstanceOfCharacter(functionName, StringLength(functionName), ']');

    std::string finalName;

    StringRange totalType{ functionName + firstBracket, functionName + lastBracket };

    size_t i = 0;

    while (totalType.mBegin < totalType.mEnd)
    {
      auto token = GetToken(totalType);

      if (i < 2)
      {
        ++token.mEnd;
      }
      else
      {
        for (auto i = token.mBegin; i < token.mEnd; ++i)
        {
          finalName.push_back(*i);
        }
      }

      totalType.mBegin = token.mEnd;
      ++i;
    }

#elif defined(_MSC_VER)
    CONSTEXPR size_t firstArrow = GetFirstInstanceOfCharacter(functionName, StringLength(functionName), '<') + 1;
    CONSTEXPR size_t lastArrow = GetLastInstanceOfCharacter(functionName, StringLength(functionName), '>');

    CONSTEXPR size_t typenameTotalRangeSize = lastArrow - firstArrow;

    ConstexprTokenWriter<typenameTotalRangeSize + 1> finalName;

    StringRange totalType{ functionName + firstArrow, functionName + lastArrow };

    while (totalType.mBegin < totalType.mEnd)
    {
      auto token = GetToken(totalType);

      if (token == "struct" || token == "class")
      {
        ++token.mEnd;
      }
      else if (token.BeginsWith("struct"))
      {
        StringRange tokenToWrite{ token.mBegin + sizeof("struct"), token.mEnd };
        finalName.Write(tokenToWrite);
      }
      else if (token.BeginsWith("class"))
      {
        StringRange tokenToWrite{ token.mBegin + sizeof("class"), token.mEnd };
        finalName.Write(tokenToWrite);
      }
      else
      {
        finalName.Write(token);
      }

      totalType.mBegin = token.mEnd;
    }
#endif

    return finalName;
  }
}

#undef CONSTEXPR
