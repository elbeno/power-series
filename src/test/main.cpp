#include "monoidal_zip.hpp"
#include "series_mult.hpp"

#include "range/v3/all.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace ranges;

namespace power_series
{
  template <typename T>
  auto series(T t)
  {
    return view::concat(view::single(t), view::repeat(0));
  }

  template <typename Rng>
  auto negate(Rng&& r)
  {
    return view::transform(std::forward<Rng>(r),
                           [] (auto i) { return -i; });
  }

  template <typename R1, typename R2>
  auto add(R1&& r1, R2&& r2)
  {
    return view::monoidal_zip(plus<>(),
                              std::forward<R1>(r1),
                              std::forward<R2>(r2),
                              0);
  }

  template <typename R1, typename R2>
  auto sub(R1&& r1, R2&& r2)
  {
    return add(std::forward<R1>(r1),
               negate(std::forward<R2>(r2)));
  }

  template <typename R1, typename R2>
  auto simple_mult(R1&& r1, R2&& r2)
  {
    return view::monoidal_zip(multiplies<>(),
                              std::forward<R1>(r1),
                              std::forward<R2>(r2),
                              1);
  }

  template <typename R1, typename R2>
  auto mult(R1&& r1, R2&& r2)
  {
    return view::series_mult(std::forward<R1>(r1),
                             std::forward<R2>(r2));
  }

}

int main(int, char* [])
{
  {
    vector<int> v1{1, 2, 3, 4, 5};
    vector<int> v2{1, 2, 3, 4, 5};

    auto m = power_series::mult(v2, v1);

    ranges::for_each(m, [](int i){
        cout << i << ' ';
      });
    cout << endl;
  }

  {
    vector<string> v1{"a", "b", "c", "d", "e"};
    vector<string> v2{"1", "2", "3", "4"};

    auto r = view::monoidal_zip([] (const string& a, const string& b)
                                { return a + b; }, v1, v2, string());

    ranges::for_each(r, [](const string& i){
        cout << i << ' ';
      });
    cout << endl;
  }
}
