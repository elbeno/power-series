#include "power_series.hpp"
#include "iterate.hpp"
#include "iterate_n.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace ranges;

std::string x_to_power(int n)
{
  if (n == 0) return {};
  if (n == 1) return {"x"};
  return {"x^" + std::to_string(n)};
}

template <typename Rng>
void prettyprint(Rng&& r)
{
  bool start = true;

  auto m = view::zip(std::forward<Rng>(r),
                     view::iota(0))
    | view::transform(
        [&start] (const std::pair<int, int>& p) -> std::string {
          std::string s;
          if (p.first != 0) {
            if (!start)
              s = p.first > 0 ? " + " : " - ";
            s += std::to_string(p.first) + x_to_power(p.second);
            start = false;
          }
          return s;
        });

  ranges::for_each(m, [](const string& s){ cout << s; });
  cout << endl;
}

void iterate_test()
{
  // 0 1 2 3 4 5 6 7 8 9
  auto m = view::iterate([] (int x) { return x + 1; }, 0)
    | view::take(10);

  ranges::for_each(m, [](int i){
      cout << i << ' ';
    });
  cout << endl;
}

void iterate_n_test()
{
  // 0 1 2 3 4 5 6 7 8 9
  auto m = view::iterate_n([] (int x) { return x + 1; }, 0, 10);

  ranges::for_each(m, [](int i){
      cout << i << ' ';
    });
  cout << endl;
}

void monoidal_zip_test()
{
  // a1 b2 c3 d4 e
  vector<string> v1{"a", "b", "c", "d", "e"};
  vector<string> v2{"1", "2", "3", "4"};

  auto r = view::monoidal_zip([] (const string& a, const string& b)
                              { return a + b; }, v1, v2, string());

  ranges::for_each(r, [](const string& i){
      cout << i << ' ';
    });
  cout << endl;
}

void ps_add_test()
{
  // 2 4 6 8 10
  vector<int> v1{1, 2, 3, 4, 5};
  vector<int> v2{1, 2, 3, 4, 5};
  prettyprint(power_series::add(v1, v2));
}

void ps_sub_test()
{
  // 2 4 6 8 10
  vector<int> v1{3, 6, 9, 12, 15};
  vector<int> v2{1, 2, 3, 4, 5};
  prettyprint(power_series::sub(v1, v2));
}

void ps_mult_test()
{
  // 1 4 10 20 35 44 46 40 25
  vector<int> v1{1, 2, 3, 4, 5};
  vector<int> v2{1, 2, 3, 4, 5};
  prettyprint(power_series::mult(v1, v2));
}

void ps_diff_test()
{
  // 2 6
  vector<int> v1{1, 2, 3};
  prettyprint(power_series::diff(v1));
}

int main(int, char* [])
{
  monoidal_zip_test();
  iterate_test();
  iterate_n_test();

  ps_add_test();
  ps_sub_test();
  ps_mult_test();
  ps_diff_test();
}
