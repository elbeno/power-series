#include "iterate.hpp"
#include "iterate_n.hpp"

#include <range/v3/all.hpp>

#include <testinator.h>

#include <string>

using namespace std;
using namespace ranges;

// -----------------------------------------------------------------------------
// Tests for iterate and iterate_n

DEF_TEST(PlusOne, Iterate)
{
  auto m = view::take(view::iterate([] (int x) { return x + 1; }, 0), 5);
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, int x) {
        return s + to_string(x);
      });
  EXPECT(s == "01234");
  return true;
}

int collatz(int n)
{
  if (n & 1) return 3*n+1;
  return n>>1;
}

DEF_TEST(Collatz27, Iterate)
{
  auto m = view::iterate(collatz, 27)
    | view::take_while([] (int n) { return n != 1; });
  EXPECT(ranges::find(m, 9232) != ranges::end(m));
  return true;
}

DEF_TEST(Collatz6, Iterate)
{
  auto m = view::iterate(collatz, 6)
    | view::take_while([] (int n) { return n != 1; });
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, int x) {
        return s + to_string(x);
      });
  EXPECT(s == "6310516842");
  return true;
}

DEF_TEST(PlusOneN, IterateN)
{
  auto m = view::iterate_n([] (int x) { return x + 1; }, 0, 5);
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, int x) {
        return s + to_string(x);
      });
  EXPECT(s == "01234");
  return true;
}

