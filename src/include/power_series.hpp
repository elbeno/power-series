#pragma once

#include "iterate_n.hpp"
#include "monoidal_zip.hpp"
#include "series_mult.hpp"

#include <range/v3/core.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/tail.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/zip_with.hpp>

#include <functional>
#include <string>

namespace power_series
{
  template <typename Rng>
  inline auto negate(Rng&& r)
  {
    return ranges::view::transform(std::forward<Rng>(r),
                                   [] (auto i) { return -i; });
  }

  template <typename R1, typename R2>
  inline auto add(R1&& r1, R2&& r2)
  {
    return ranges::view::monoidal_zip(std::plus<>(),
                                      std::forward<R1>(r1),
                                      std::forward<R2>(r2));
  }

  template <typename R1, typename R2>
  inline auto subtract(R1&& r1, R2&& r2)
  {
    return ranges::view::monoidal_zip(std::minus<>(),
                                      std::forward<R1>(r1),
                                      std::forward<R2>(r2));
  }

  template <typename R1, typename R2>
  inline auto multiply(R1&& r1, R2&& r2)
  {
    return ranges::view::series_mult(std::forward<R1>(r1),
                                     std::forward<R2>(r2));
  }

  template <typename Rng>
  inline auto differentiate(Rng&& r)
  {
    return ranges::view::zip_with(std::multiplies<>(),
                                  ranges::view::iota(1),
                                  ranges::view::tail(std::forward<Rng>(r)));
  }

  template <typename Rng>
  inline auto integrate(Rng&& r)
  {
    return ranges::view::concat(
        ranges::view::single(0),
        ranges::view::zip_with([] (auto x, auto y) { return (float)x / (float)y; },
                               std::forward<Rng>(r),
                               ranges::view::iota(1)));
  }

  namespace detail
  {
    inline std::string x_to_power(int n)
    {
      if (n == 0) return {};
      if (n == 1) return {"x"};
      return {"x^" + std::to_string(n)};
    }

    template <typename Rng>
    inline std::string to_string(Rng&& r)
    {
      return ranges::accumulate(
          std::forward<Rng>(r),
          std::string(),
          [] (std::string s, const auto& p) {
            // if the coefficient is non-zero
            if (p.first != 0)
            {
              // treat the first one printed specially
              if (s.empty())
                s += p.first > 0 ? "" : "-";
              else
                s += p.first > 0 ? " + " : " - ";

              auto coeff = p.first > 0 ? p.first : -p.first;
              // don't print a coefficient of 1 (unless it's the constant)
              if (coeff != 1 || p.second == 0)
              {
                s += std::to_string(coeff);
              }
              s += detail::x_to_power(p.second);
            }
            return s;
          });
    }
  }

  template <typename Rng>
  inline std::string to_string(Rng&& r)
  {
    return detail::to_string(
        ranges::view::zip(std::forward<Rng>(r),
                          ranges::view::iota(0)));
  }

  template <typename Rng>
  inline std::string to_string_reverse(Rng&& r)
  {
    auto s = ranges::size(r);
    auto powers = ranges::view::iterate_n([] (auto i) { return i - 1; }, s-1, s);
    return detail::to_string(
        ranges::view::zip(ranges::view::reverse(std::forward<Rng>(r)), powers));
  }

}
