#include "power_series.hpp"
#include "iterate.hpp"
#include "iterate_n.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace ranges;

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

  auto m = power_series::add(v1, v2);

  ranges::for_each(m, [](int i){
      cout << i << ' ';
    });
  cout << endl;
}

void ps_sub_test()
{
  // 2 4 6 8 10
  vector<int> v1{3, 6, 9, 12, 15};
  vector<int> v2{1, 2, 3, 4, 5};

  auto m = power_series::sub(v1, v2);

  ranges::for_each(m, [](int i){
      cout << i << ' ';
    });
  cout << endl;
}

void ps_mult_test()
{
  // 1 4 10 20 35 44 46 40 25
  vector<int> v1{1, 2, 3, 4, 5};
  vector<int> v2{1, 2, 3, 4, 5};

  auto m = power_series::mult(v1, v2);

  ranges::for_each(m, [](int i){
      cout << i << ' ';
    });
  cout << endl;
}

int main(int, char* [])
{
  ps_add_test();
  ps_sub_test();
  ps_mult_test();

  monoidal_zip_test();

  iterate_test();
  iterate_n_test();
}
