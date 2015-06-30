#include "scan.hpp"

#include <range/v3/all.hpp>

#include <testinator.h>

#include <string>
#include <vector>

using namespace std;
using namespace ranges;

// -----------------------------------------------------------------------------
// Tests for scan

DEF_TEST(BasicScan, Scan)
{
  vector<int> v1 = {1, 2, 3};
  auto m = view::scan(v1, 0);
  string s = ranges::accumulate(
      m,
      string(),
      [] (string s, int x) {
        return s + to_string(x);
      });
  EXPECT(s == "0136");
  return true;
}
