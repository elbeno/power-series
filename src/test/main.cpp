#include "monoidal_zip.hpp"

#include <range/v3/all.hpp>

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

using namespace ranges;

int main(int, char* [])
{
  {
    vector<int> v1{1, 2, 3, 4, 5};
    vector<int> v2{100, 200, 300, 400};

    auto r = view::monoidal_zip(plus<>(), v1, v2, 0);

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
