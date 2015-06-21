#include "monoidal_zip.hpp"

#include <range/v3/all.hpp>

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
    auto h1 = *ranges::begin(std::forward<R1>(r1));
    auto h2 = *ranges::begin(std::forward<R2>(r2));

    auto head = h1 * h2;
    /*return view::concat(view::single(head),
                        add(mult(view::tail(r1), r2),
                        mult(view::tail(r2), series(h1))));*/
    return 0;
  }

}

int main(int, char* [])
{
  {
    vector<int> v1{1, 2, 3, 4, 5};
    vector<int> v2{100, 200, 300, 400};

    auto r = power_series::add(v1, v2);
    auto m = power_series::mult(v1, v2);

    ranges::for_each(r, [](int i){
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
