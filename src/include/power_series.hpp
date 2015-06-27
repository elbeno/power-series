#pragma once

#include "monoidal_zip.hpp"
#include "series_mult.hpp"

#include <range/v3/view/concat.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/tail.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip_with.hpp>

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
  auto subtract(R1&& r1, R2&& r2)
  {
    return ranges::view::monoidal_zip(std::minus<>(),
                                      std::forward<R1>(r1),
                                      std::forward<R2>(r2),
                                      0);
  }

  template <typename R1, typename R2>
  auto multiply(R1&& r1, R2&& r2)
  {
    return ranges::view::series_mult(std::forward<R1>(r1),
                                     std::forward<R2>(r2));
  }

  template <typename Rng>
  auto differentiate(Rng&& r)
  {
    return ranges::view::zip_with(std::multiplies<>(),
                                  ranges::view::iota(1),
                                  ranges::view::tail(std::forward<Rng>(r)));
  }

  template <typename Rng>
  auto integrate(Rng&& r)
  {
    return ranges::view::concat(
        ranges::view::single(0),
        ranges::view::zip_with([] (auto x, auto y) { return (float)x / (float)y; },
                               std::forward<Rng>(r),
                               ranges::view::iota(1)));
  }

}
