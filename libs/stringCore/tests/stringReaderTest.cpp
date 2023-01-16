#include <motifCpp/testCheck.h>
#include <stringcore/stringreader.h>

using namespace Mso::String::ReadOps;

TEST_CLASS (StringReaderTests)
{
  TEST_METHOD(EndIsTrueForEmpty)
  {
    Mso::String::StringReader<char> reader("");
    TestAssert::IsTrue(reader.TryRead(End()));
  }

  TEST_METHOD(ExpectCharIsFalseForEmpty)
  {
    Mso::String::StringReader<char> reader("");
    TestAssert::IsFalse(reader.TryRead(ExpectChar('-')));
  }

  TEST_METHOD(ExpectChars)
  {
    Mso::String::StringReader<char> reader("abc");
    TestAssert::IsTrue(reader.TryRead(ExpectChar('a'), ExpectChar('b'), ExpectChar('c'), End()));
  }

  TEST_METHOD(ExpectHex)
  {
    Mso::String::StringReader<char> reader("AB");

    uint8_t value = 0;
    TestAssert::IsTrue(reader.TryRead(GetHex(value), End()));
    TestAssert::AreEqual(0xAB, value);
  }
};
