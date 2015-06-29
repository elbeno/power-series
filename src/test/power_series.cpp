#include "power_series.hpp"

#include <range/v3/all.hpp>

#include <testinator.h>

#include <string>
#include <vector>

using namespace std;
using namespace ranges;

// -----------------------------------------------------------------------------
// Tests for power_series

// -----------------------------------------------------------------------------
// Addition

DEF_TEST(AddSeries, PowerSeries)
{
  vector<int> v1{1, 2, 3, 4, 5};
  vector<int> v2{1, 2, 3, 4, 5};
  string s = power_series::to_string(power_series::add(v1, v2));
  EXPECT(s == "2 + 4x + 6x^2 + 8x^3 + 10x^4");
  return true;
}

DEF_TEST(AddSeriesInfinite, PowerSeries)
{
  auto m = ranges::view::iota(1);
  auto n = ranges::view::iota(1);
  auto a = power_series::add(m, n);
  string s = power_series::to_string(view::take(a, 3));
  EXPECT(s == "2 + 4x + 6x^2");
  return true;
}

// -----------------------------------------------------------------------------
// Negation

DEF_TEST(NegateSeries, PowerSeries)
{
  vector<int> v1{1, 2, 3, 4, 5};
  string s = power_series::to_string(power_series::negate(v1));
  EXPECT(s == "-1 - 2x - 3x^2 - 4x^3 - 5x^4");
  return true;
}

DEF_TEST(NegateSeriesInfinite, PowerSeries)
{
  auto n = ranges::view::iota(1);
  auto a = power_series::negate(n);
  string s = power_series::to_string(view::take(a, 3));
  EXPECT(s == "-1 - 2x - 3x^2");
  return true;
}

// -----------------------------------------------------------------------------
// Subtraction

DEF_TEST(SubtractSeries, PowerSeries)
{
  vector<int> v1{2, 3, 4, 5, 6};
  vector<int> v2{1, 2, 3, 4, 5};
  string s = power_series::to_string(power_series::subtract(v1, v2));
  EXPECT(s == "1 + x + x^2 + x^3 + x^4");
  return true;
}

DEF_TEST(SubtractSeriesInfinite, PowerSeries)
{
  auto m = ranges::view::iota(2);
  auto n = ranges::view::iota(1);
  auto a = power_series::subtract(m, n);
  string s = power_series::to_string(view::take(a, 3));
  EXPECT(s == "1 + x + x^2");
  return true;
}

// -----------------------------------------------------------------------------
// Multiplication

DEF_TEST(MultiplySeriesEqual, PowerSeries)
{
  vector<int> v1{1, 2, 3, 4, 5};
  vector<int> v2{1, 2, 3, 4, 5};
  string s = power_series::to_string(power_series::multiply(v1, v2));
  EXPECT(s == "1 + 4x + 10x^2 + 20x^3 + 35x^4 + 44x^5 + 46x^6 + 40x^7 + 25x^8");
  return true;
}

DEF_TEST(MultiplySeriesLargerFirst, PowerSeries)
{
  vector<int> v1{1, 2, 3, 4, 5};
  vector<int> v2{1, 2, 3};
  string s = power_series::to_string(power_series::multiply(v1, v2));
  EXPECT(s == "1 + 4x + 10x^2 + 16x^3 + 22x^4 + 22x^5 + 15x^6");
  return true;
}

DEF_TEST(MultiplySeriesLargerSecond, PowerSeries)
{
  vector<int> v1{1, 2, 3};
  vector<int> v2{1, 2, 3, 4, 5};
  string s = power_series::to_string(power_series::multiply(v1, v2));
  EXPECT(s == "1 + 4x + 10x^2 + 16x^3 + 22x^4 + 22x^5 + 15x^6");
  return true;
}

DEF_TEST(MultiplySeriesReversible, PowerSeries)
{
  vector<int> v1{1, 1};
  vector<int> v2{1, 1};
  string s = power_series::to_string_reverse(power_series::multiply(v1, v2));
  EXPECT(s == "x^2 + 2x + 1");
  return true;
}

// -----------------------------------------------------------------------------
// Differentiation

DEF_TEST(DifferentiateSeries, PowerSeries)
{
  vector<int> v1{1, 2, -3};
  string s = power_series::to_string(power_series::differentiate(v1));
  EXPECT(s == "2 - 6x");
  return true;
}

// -----------------------------------------------------------------------------
// Integration

DEF_TEST(IntegrateSeries, PowerSeries)
{
  vector<int> v1{1, 1, 1};
  auto a = power_series::integrate(v1);
  EXPECT(ranges::at(a, 0) == 0);
  EXPECT(ranges::at(a, 1) == 1);
  EXPECT(ranges::at(a, 2) == 1.f/2.f);
  EXPECT(ranges::at(a, 3) == 1.f/3.f);
  return true;
}

// -----------------------------------------------------------------------------
// Printing

DEF_TEST(ToString1, PowerSeries)
{
  vector<int> v1{1, 2, 3};
  string s = power_series::to_string(v1);
  EXPECT(s == "1 + 2x + 3x^2");
  return true;
}

DEF_TEST(ToString2, PowerSeries)
{
  vector<int> v1{-1, 1, 3};
  string s = power_series::to_string(v1);
  EXPECT(s == "-1 + x + 3x^2");
  return true;
}

DEF_TEST(ToString3, PowerSeries)
{
  vector<int> v1{0, 2, 3};
  string s = power_series::to_string(v1);
  EXPECT(s == "2x + 3x^2");
  return true;
}

DEF_TEST(ToString4, PowerSeries)
{
  vector<int> v1{0, -2, 3};
  string s = power_series::to_string(v1);
  EXPECT(s == "-2x + 3x^2");
  return true;
}

DEF_TEST(ToString5, PowerSeries)
{
  vector<int> v1{1, 0, 3};
  string s = power_series::to_string(v1);
  EXPECT(s == "1 + 3x^2");
  return true;
}

DEF_TEST(ToString6, PowerSeries)
{
  vector<int> v1{1, 0, -3};
  string s = power_series::to_string(v1);
  EXPECT(s == "1 - 3x^2");
  return true;
}

DEF_TEST(ToString7, PowerSeries)
{
  vector<int> v1{1, 0, -3};
  string s = power_series::to_string_reverse(v1);
  EXPECT(s == "-3x^2 + 1");
  return true;
}
