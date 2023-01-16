/**
  StringReader is a simple operation-based class for pulling data out of strings.
  TryRead accepts an arbitrary number of operations to perform.
*/
#pragma once
#include <stringCore/msostringTraits.h>
#include <stringCore/string_adapter.h>
#include <optional>

namespace Mso::String {

/**
  Parse a single hex digit
*/
template <typename TChar>
static std::optional<uint8_t> TryParseHexChar(TChar ch) noexcept
{
  if (ch >= '0' && ch <= '9')
  {
    return static_cast<uint8_t>(ch - '0');
  }
  else if (ch >= 'A' && ch <= 'F')
  {
    return static_cast<uint8_t>(ch - 'A' + 10);
  }
  else if (ch >= 'a' && ch <= 'f')
  {
    return static_cast<uint8_t>(ch - 'a' + 10);
  }
  else
  {
    return {};
  }
}

/**
  ReadOperations are used to read from the strings.
  Operations implement TryRead and update the input on success.
*/
namespace ReadOps {

/**
  returns true if the input is empty
*/
struct End
{
  End() noexcept = default;

  template <typename TChar>
  bool TryRead(Mso::basic_string_view<TChar>& input) const noexcept
  {
    return input.empty();
  }
};

/**
  returns true if the next char matches the value
*/
template <typename TChar>
struct ExpectChar
{
  ExpectChar(TChar value) noexcept : m_value(value) {}

  template <typename TChar2>
  bool TryRead(Mso::basic_string_view<TChar2>& input) const noexcept
  {
    if (input.size() < 1)
      return false;

    if (input.front() != m_value)
      return false;

    input.remove_prefix(1);
    return true;
  }

  const TChar m_value;
};

/**
  Parse the matching number of characters as a hex value
*/
template <typename T>
struct GetHex
{
  GetHex(T& value) noexcept : m_value(value) {}

  template <typename TChar>
  bool TryRead(Mso::basic_string_view<TChar>& input) const noexcept
  {
    static const size_t c_cchHex = Mso::String::HexTraits<T>::CharCount;
    if (input.size() < c_cchHex)
      return false;

    m_value = 0;
    for (auto ch : input.substr(0, c_cchHex))
    {
      auto optValue = TryParseHexChar(ch);
      if (!optValue)
        return false;

      m_value = (m_value << 4) + *optValue;
    }

    input.remove_prefix(c_cchHex);
    return true;
  }

  T& m_value;
};

} // namespace ReadOps

template <typename TChar>
class StringReader
{
public:
  StringReader(const Mso::basic_string_view<TChar>& input) noexcept : m_input(input) {}

  static bool TryRead() noexcept
  {
    return true;
  }

  /**
    If the next characters parse into N hex values, move foward and return the values
  */
  template <typename TReadOp, typename... TReadOps>
  bool TryRead(const TReadOp& readOp, const TReadOps&... readOps) noexcept
  {
    if (!readOp.TryRead(m_input))
      return false;

    return TryRead(readOps...);
  }

  bool IsEmpty() const noexcept
  {
    return m_input.empty();
  }

private:
  Mso::basic_string_view<TChar> m_input;
};

} // namespace Mso::String
