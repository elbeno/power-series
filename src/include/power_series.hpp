#pragma once

#include "monoidal_zip.hpp"
#include "series_mult.hpp"
#include "range/v3/all.hpp"

#include <functional>

namespace power_series
{
  template <typename Rng>
  auto negate(Rng&& r)
  {
    return ranges::view::transform(std::forward<Rng>(r),
                                   [] (auto i) { return -i; });
  }

  template <typename R1, typename R2>
  auto add(R1&& r1, R2&& r2)
  {
    return ranges::view::monoidal_zip(std::plus<>(),
                                      std::forward<R1>(r1),
                                      std::forward<R2>(r2),
                                      0);
  }

  template <typename R1, typename R2>
  auto sub(R1&& r1, R2&& r2)
  {
    return ranges::view::monoidal_zip(std::minus<>(),
                                      std::forward<R1>(r1),
                                      std::forward<R2>(r2),
                                      0);
  }

  template <typename R1, typename R2>
  auto mult(R1&& r1, R2&& r2)
  {
    return ranges::view::series_mult(std::forward<R1>(r1),
                                     std::forward<R2>(r2));
  }
}
