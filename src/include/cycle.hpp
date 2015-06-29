#pragma once

#include <range/v3/core.hpp>

namespace ranges
{
  inline namespace v3
  {
    template<typename Rng>
    struct cycle_view
      : view_facade<cycle_view<Rng>, infinite>
    {
    private:
      friend struct range_access;
      using difference_type_ = range_difference_t<Rng>;
      using size_type_ = meta::eval<std::make_unsigned<difference_type_>>;
      Rng r_;

      template <bool IsConst>
      struct cursor
      {
      private:
        template <typename T>
        using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
        using cycle_view_t = constify_if<cycle_view>;
        cycle_view_t *rng_;
        range_iterator_t<constify_if<Rng>> it_;

      public:
        using single_pass = std::false_type;
        cursor() = default;
        cursor(cycle_view_t &rng)
          : rng_{&rng}
          , it_{begin(rng.r_)}
        {}
        constexpr bool done() const
        {
          return false;
        }
        auto current() const
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
          *it_
        )
        void next()
        {
          if (++it_ == end(rng_->r_))
            it_ = begin(rng_->r_);
        }
        CONCEPT_REQUIRES((bool) BidirectionalRange<Rng>())
        void prev()
        {
          if (it_ == begin(rng_->r_))
            it_ = end(rng_->r_);
          --it_;
        }
        bool equal(cursor const &that) const
        {
          return it_ == that.it_;
        }
      };

      cursor<false> begin_cursor()
      {
        return {*this};
      }
      CONCEPT_REQUIRES((bool) Range<Rng const>())
      cursor<true> begin_cursor() const
      {
        return {*this};
      }

    public:
      cycle_view() = default;
      explicit cycle_view(Rng r)
        : r_(std::move(r))
      {}
    };

    namespace view
    {
      struct cycle_fn
      {
        template<typename Rng,
                 CONCEPT_REQUIRES_(ForwardRange<Rng>())>
        cycle_view<all_t<Rng>> operator()(Rng && r) const
        {
          return cycle_view<all_t<Rng>>{all(std::forward<Rng>(r))};
        }

#ifndef RANGES_DOXYGEN_INVOKED
        template<typename Rng,
                 CONCEPT_REQUIRES_(!ForwardRange<Rng>())>
        void operator()(Rng && r) const
        {
          CONCEPT_ASSERT_MSG(
              ForwardRange<Rng>(),
              "The range passed to view::cycle must model the ForwardRange concept.");
        }
#endif
      };

      namespace
      {
        constexpr auto&& cycle = static_const<cycle_fn>::value;
      }
    }
  }
}
