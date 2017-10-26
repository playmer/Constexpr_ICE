#include <iostream>

#include <initializer_list>

#define YTEParameterNames(...) {__VA_ARGS__}

template <size_t tSize>
size_t GiveSize()
{
  return tSize;
}

int main()
{
  auto thing1 = GiveSize<std::initializer_list<const char*>YTEParameterNames("Woah", "Jeez").size()>(); // Okay
  auto thing2 = GiveSize<(std::initializer_list<const char*>)YTEParameterNames("Woah", "Jeez").size()>(); // Whoops
}