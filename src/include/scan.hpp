#pragma once

#include <range/v3/utility/semiregular.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/numeric/accumulate.hpp>

namespace ranges
{
  inline namespace v3
  {
    namespace detail
    {
      template<typename C>
      using scan_cardinality =
        std::integral_constant<cardinality,
          C::value == infinite ?
            infinite :
            C::value == unknown ?
              unknown :
              C::value >= 0 ?
                static_cast<ranges::cardinality>(C::value + 1) :
                finite>;
    } // namespace detail

    template<typename Rng, typename T, typename Op, typename P>
    struct scan_view
      : view_facade<scan_view<Rng, T, Op, P>,
                    detail::scan_cardinality<range_cardinality<Rng>>::value>
    {
    private:
      friend struct range_access;
      Rng r_;
      T init_;
      semiregular_t<function_type<Op>> op_;
      semiregular_t<function_type<P>> proj_;

      using difference_type_ = range_difference_t<Rng>;
      using size_type_ = meta::eval<std::make_unsigned<difference_type_>>;

      template <bool IsConst>
      struct sentinel;

      template <bool IsConst>
      struct cursor
      {
        using difference_type = range_difference_t<Rng>;
      private:
        friend struct sentinel<IsConst>;
        template <typename C>
        using constify_if = meta::apply<meta::add_const_if_c<IsConst>, C>;
        using scan_view_t = constify_if<scan_view>;
        scan_view_t *rng_;
        range_iterator_t<constify_if<Rng>> it_;
        T val_;
        bool done_;
      public:
        cursor() = default;
        cursor(scan_view_t &rng)
          : rng_{&rng}
          , it_{begin(rng.r_)}
          , val_{rng.init_}
          , done_{false}
        {}
        auto current() const
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
          val_
        )
        void next()
        {
          update_value();
          if (it_ != end(rng_->r_))
            ++it_;
          else
            done_ = true;
        }
        bool equal(cursor const &that) const
        {
          return it_ == that.it_;
        }
      private:
        void update_value()
        {
          val_ = rng_->op_(val_, rng_->proj_(*it_));
        }
        bool done() const
        {
          return done_;
        }
      };

      template <bool IsConst>
      struct sentinel
      {
      private:
        template <typename C>
        using constify_if = meta::apply<meta::add_const_if_c<IsConst>, C>;
        using scan_view_t = constify_if<scan_view>;
      public:
        sentinel() = default;
        sentinel(scan_view_t &)
        {}
        bool equal(cursor<IsConst> const &pos) const
        {
          return pos.done();
        }
      };

      cursor<false> begin_cursor()
      {
        return {*this};
      }
      sentinel<false> end_cursor()
      {
        return {*this};
      }

      CONCEPT_REQUIRES((bool) Range<Rng const>())
      cursor<true> begin_cursor()
      {
        return {*this};
      }
      CONCEPT_REQUIRES((bool) Range<Rng const>())
      sentinel<true> end_cursor()
      {
        return {*this};
      }

    public:
      scan_view() = default;
      explicit scan_view(Rng r, T t, Op op, P proj)
        : r_(std::move(r))
        , init_(std::move(t))
        , op_(as_function(std::move(op)))
        , proj_(as_function(std::move(proj)))
      {}
      CONCEPT_REQUIRES((bool) SizedRange<Rng>())
      constexpr size_type_ size() const
      {
        return detail::scan_cardinality<range_cardinality<Rng>>::value > 0 ?
          static_cast<size_type_>(detail::scan_cardinality<range_cardinality<Rng>>::value) :
          ranges::size(r_) + 1;
      }
    };

    namespace view
    {
      struct scan_fn
      {
        template<typename Rng, typename T, typename Op, typename P>
        using Concept = meta::and_<InputRange<Rng>,
                                   Accumulateable<range_iterator_t<Rng>, T, Op, P>>;

        template<typename Rng, typename T, typename Op = plus, typename P = ident,
                 CONCEPT_REQUIRES_(Concept<Rng, T, Op, P>())>
        scan_view<all_t<Rng>, T, Op, P> operator()(
            Rng&& r, T init, Op op = Op{}, P proj = P{}) const
        {
          return scan_view<all_t<Rng>, T, Op, P>{
              all(std::forward<Rng>(r)),
              std::move(init),
              std::move(op),
              std::move(proj)
          };
        }
      };

      namespace
      {
        constexpr auto&& scan = static_const<with_braced_init_args<scan_fn>>::value;
      }
    }
  }
}
