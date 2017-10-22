#pragma once

#include <array>

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
  inline CONSTEXPR size_t StringLength(const char *aString)
  {
    size_t i = 0;

    while (*aString != '\0')
    {
      ++i;
      ++aString;
    }

    return i;
  }

  template <size_t tConstSize>
  struct ConstexprToken
  {
  public:
    CONSTEXPR ConstexprToken()
    {
    }

    CONSTEXPR ConstexprToken(const char *aBegin)
    {
      for (size_t i = 0; i < tConstSize; i++)
      {
        mData[i] = aBegin[i];
      }

      mData[tConstSize] = { '\0' };
    }

    template<size_t tDifferentSize>
    CONSTEXPR ConstexprToken(const ConstexprToken<tDifferentSize> &aToken)
    {
      static_assert(tDifferentSize <= tConstSize, "Trying to copy a ConstexprToken to a ConstexprToken of too small a size.");
      for (size_t i = 0; i < tDifferentSize; i++)
      {
        mData[i] = aToken.data()[i];
      }

      mData[tDifferentSize] = '\0';
    }

    CONSTEXPR size_t Size() const { return tConstSize; };
    CONSTEXPR size_t size() const { return tConstSize; };
    CONSTEXPR const char* Data() const { return mData.data(); };
    CONSTEXPR const char* data() const { return mData.data(); };

    std::array<char, tConstSize + 1> mData;
  };


  struct StringRange
  {
    CONSTEXPR StringRange(const char *aBegin, const char *aEnd)
      : mBegin(aBegin)
      , mEnd(aEnd)
    {

    }

    CONSTEXPR StringRange(const char *aBegin)
      : mBegin(aBegin),
      mEnd(aBegin + StringLength(aBegin))
    {

    }

    constexpr bool BeginsWith(const StringRange &aRight) const
    {
      if (Size() >= aRight.Size())
      {
        for (size_t i = 0; i < aRight.Size(); ++i)
        {
          if (mBegin[i] != aRight.mBegin[i])
          {
            return false;
          }
        }

        return true;
      }

      return false;
    }

    constexpr bool operator==(const StringRange &aRight) const
    {
      if (Size() == aRight.Size())
      {
        for (size_t i = 0; i < Size(); ++i)
        {
          if (mBegin[i] != aRight.mBegin[i])
          {
            return false;
          }
        }

        return true;
      }

      return false;
    }

    CONSTEXPR size_t Size() const
    {
      return mEnd - mBegin;
    }

    const char *mBegin;
    const char *mEnd;
  };



  template<size_t tConstSize>
  struct ConstexprTokenWriter : public ConstexprToken<tConstSize>
  {
    CONSTEXPR ConstexprTokenWriter()
      : mWritingPosition(this->mData.data())
    {
      for (size_t i = 0; i < tConstSize; i++)
      {
        this->mData[i] = 0;
      }

      this->mData[tConstSize] = { '\0' };
    }


    CONSTEXPR void Write(StringRange aRange)
    {
      while (aRange.mBegin < aRange.mEnd)
      {
        *mWritingPosition++ = *aRange.mBegin++;
      }
    }


  private:
    char *mWritingPosition;
  };

  inline CONSTEXPR size_t GetLastInstanceOfCharacter(const char *aString, size_t aSize, char aCharacter)
  {
    size_t toReturn = aSize + 1;

    while (aSize != 0)
    {
      if (aString[aSize] == aCharacter)
      {
        toReturn = aSize;
        break;
      }
      --aSize;
    }

    return toReturn;
  }


  inline CONSTEXPR size_t GetFirstInstanceOfCharacter(const char *aString, size_t aSize, char aCharacter)
  {
    size_t toReturn = aSize + 1;

    size_t i = 0;

    while (i != aSize)
    {
      if (aString[i] == aCharacter)
      {
        toReturn = i;
        break;
      }
      ++i;
    }

    return toReturn;
  }
}

#undef CONSTEXPR
