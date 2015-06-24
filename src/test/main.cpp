#include "power_series.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace ranges;

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
