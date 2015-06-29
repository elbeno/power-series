#include "monoidal_zip.hpp"

#include <range/v3/all.hpp>

#include <testinator.h>

#include <string>
#include <vector>

using namespace std;
using namespace ranges;

// -----------------------------------------------------------------------------
// Tests for monoidal_zip

DEF_TEST(EqualCardinality, MonoidalZip)
{
  vector<string> v1{"a", "b", "c", "d"};
  vector<string> v2{"1", "2", "3", "4"};

  auto m = view::monoidal_zip(
      [] (const string& a, const string& b) {
        return a + b;
      },
      v1, v2);
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, const string& x) {
        return s + x;
      });
  EXPECT(s == "a1b2c3d4");
  return true;
}

DEF_TEST(EqualCardinalityReverse, MonoidalZip)
{
  vector<string> v1{"a", "b", "c", "d"};
  vector<string> v2{"1", "2", "3", "4"};

  auto m = view::reverse(
      view::monoidal_zip(
          [] (const string& a, const string& b) {
            return a + b;
          },
          v1, v2));
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, const string& x) {
        return s + x;
      });
  EXPECT(s == "d4c3b2a1");
  return true;
}

DEF_TEST(LargerFirst, MonoidalZip)
{
  vector<string> v1{"a", "b", "c", "d", "e"};
  vector<string> v2{"1", "2", "3", "4"};

  auto m = view::monoidal_zip(
      [] (const string& a, const string& b) {
        return a + b;
      },
      v1, v2);
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, const string& x) {
        return s + x;
      });
  EXPECT(s == "a1b2c3d4e");
  return true;
}

DEF_TEST(LargerFirstReverse, MonoidalZip)
{
  vector<string> v1{"a", "b", "c", "d", "e"};
  vector<string> v2{"1", "2", "3", "4"};

  auto m = view::reverse(
      view::monoidal_zip(
          [] (const string& a, const string& b) {
            return a + b;
          },
          v1, v2));
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, const string& x) {
        return s + x;
      });
  EXPECT(s == "ed4c3b2a1");
  return true;
}

DEF_TEST(LargerSecond, MonoidalZip)
{
  vector<string> v1{"a", "b", "c", "d"};
  vector<string> v2{"1", "2", "3", "4", "5"};

  auto m = view::monoidal_zip(
      [] (const string& a, const string& b) {
        return a + b;
      },
      v1, v2);
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, const string& x) {
        return s + x;
      });
  EXPECT(s == "a1b2c3d45");
  return true;
}

DEF_TEST(LargerSecondReverse, MonoidalZip)
{
  vector<string> v1{"a", "b", "c", "d"};
  vector<string> v2{"1", "2", "3", "4", "5"};

  auto m = view::reverse(
      view::monoidal_zip(
          [] (const string& a, const string& b) {
            return a + b;
          },
          v1, v2));
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, const string& x) {
        return s + x;
      });
  EXPECT(s == "5d4c3b2a1");
  return true;
}
