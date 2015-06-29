#include "cycle.hpp"

#include <range/v3/all.hpp>

#include <testinator.h>

#include <string>
#include <vector>

using namespace std;
using namespace ranges;

// -----------------------------------------------------------------------------
// Tests for cycle

DEF_TEST(BasicCycle, Cycle)
{
  vector<int> v1 = {1, 2, 3};
  auto m = view::take(view::cycle(v1), 6);
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, int x) {
        return s + to_string(x);
      });
  EXPECT(s == "123123");
  return true;
}

DEF_TEST(CyclePrev, Cycle)
{
  vector<int> v1 = {1, 2, 3};
  auto m = view::cycle(v1);
  auto it = ranges::begin(m);
  EXPECT(*it == 1);
  --it;
  EXPECT(*it == 3);
  --it;
  EXPECT(*it == 2);
  return true;
}
