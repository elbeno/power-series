#pragma once

#include <range/v3/numeric/inner_product.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/reverse.hpp>

namespace ranges
{
  inline namespace v3
  {
    namespace detail
    {
      template<typename C1, typename C2>
      using series_mult_cardinality =
        std::integral_constant<cardinality,
          C1::value == infinite || C2::value == infinite ?
            infinite :
            C1::value == unknown || C2::value == unknown ?
              unknown :
              C1::value >= 0 && C2::value >= 0 ?
                static_cast<ranges::cardinality>(C1::value + C2::value - 1) :
                finite>;
    } // namespace detail

    template<typename R1, typename R2>
    struct iter_series_mult_view
      : view_facade<iter_series_mult_view<R1, R2>,
                    detail::series_mult_cardinality<
                      range_cardinality<R1>,
                      range_cardinality<R2>>::value>
    {
    private:
      CONCEPT_ASSERT(meta::and_<BidirectionalRange<R1>, BidirectionalRange<R2>>());
      friend range_access;
      R1 r1_;
      R2 r2_;

      using difference_type_ = common_type_t<range_difference_t<R1>,
                                             range_difference_t<R2>>;
      using size_type_ = meta::eval<std::make_unsigned<difference_type_>>;

      template <bool IsConst>
      struct sentinel;

      template <bool IsConst>
      struct cursor
      {
        using difference_type = common_type_t<range_difference_t<R1>,
                                              range_difference_t<R2>>;
      private:
        friend struct sentinel<IsConst>;
        template <typename T>
        using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
        using series_mult_view_t = constify_if<iter_series_mult_view>;
        series_mult_view_t *rng_;
        range_iterator_t<constify_if<R1>> it1_;
        range_iterator_t<constify_if<R2>> it2_;
        difference_type length_;
        difference_type diff_;
        difference_type tail_;

      public:
        cursor() = default;
        cursor(series_mult_view_t &rng, begin_tag)
          : rng_{&rng}
          , it1_{begin(rng.r1_)}
          , it2_{begin(rng.r2_)}
          , length_{0}
          , diff_{0}
          , tail_{0}
        { next(); }
        cursor(series_mult_view_t &rng, end_tag)
          : rng_{&rng}
          , it1_{end(rng.r1_)}
          , it2_{end(rng.r2_)}
          , length_{detail::min_(detail::distance_to(begin(rng.r1_), it1_),
                                 detail::distance_to(begin(rng.r2_), it2_))}
          , diff_{detail::distance_to(begin(rng.r1_), it1_) -
                detail::distance_to(begin(rng.r2_), it2_)}
          , tail_{length_}
        {}
        auto compute_current() const
        {
          auto r1 =  make_range(
              begin(rng_->r1_) + tail_ + (diff_ > 0 ? diff_ : 0),
              it1_);
          auto r2 = view::reverse(
              make_range(
                  begin(rng_->r2_) + tail_ + (diff_ < 0 ? -diff_ : 0),
                  it2_));

          return ranges::inner_product(r1, r2, 0);
        }

        auto current() const
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            compute_current()
        )
        void next()
        {
          if (tail_ > 0 || (it1_ == end(rng_->r1_) && it2_ == end(rng_->r2_)))
          {
            ++tail_;
            return;
          }
          // r1 longer than r2
          if (it2_ == end(rng_->r2_))
          {
            detail::inc(it1_);
            ++diff_;
            return;
          }
          // r2 longer than r1
          if (it1_ == end(rng_->r1_))
          {
            --diff_;
            detail::inc(it2_);
            return;
          }
          // same
          ++length_;
          detail::inc(it1_);
          detail::inc(it2_);
        }
        bool equal(cursor const &that) const
        {
          return detail::equal_to(it1_, that.it1_)
            && detail::equal_to(it2_, that.it2_)
            && length_ == that.length_
            && diff_ == that.diff_
            && tail_ == that.tail_;
        }
        CONCEPT_REQUIRES(meta::and_c<(bool) BidirectionalRange<R1>(),
                                     (bool) BidirectionalRange<R2>()>::value)
        void prev()
        {
          if (tail_ > 0)
          {
            --tail_;
            return;
          }
          // r1 longer than r2
          if (diff_ > 0)
          {
            detail::dec(it1_);
            --diff_;
            return;
          }
          // r2 longer than r1
          if (diff_ < 0)
          {
            ++diff_;
            detail::dec(it2_);
            return;
          }
          // same
          --length_;
          detail::dec(it1_);
          detail::dec(it2_);
        }
        CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessRange<R1>(),
                                     (bool) RandomAccessRange<R2>()>::value)
        difference_type distance_to(cursor const &that) const
        {
          return detail::max_(detail::distance_to(it1_, that.it1_),
                              detail::distance_to(it2_, that.it2_))
            + abs(tail_ - that.tail_);
        }
      };

      template <bool IsConst>
      struct sentinel
      {
      private:
        template <typename T>
        using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
        using series_mult_view_t = constify_if<iter_series_mult_view>;
        range_sentinel_t<constify_if<R1>> end1_;
        range_sentinel_t<constify_if<R2>> end2_;
      public:
        sentinel() = default;
        sentinel(series_mult_view_t &rng, end_tag)
          : end1_(end(rng.r1_))
          , end2_(end(rng.r2_))
        {}
        bool equal(cursor<IsConst> const &pos) const
        {
          return detail::equal_to(pos.it1_, end1_)
            && detail::equal_to(pos.it2_, end2_)
            && pos.tail_ == pos.length_;
        }
      };

      using are_bounded_t = meta::and_c<(bool) BoundedRange<R1>(),
                                        (bool) BoundedRange<R2>()>;

      cursor<false> begin_cursor()
      {
        return {*this, begin_tag{}};
      }
      meta::if_<are_bounded_t, cursor<false>, sentinel<false>>
      end_cursor()
      {
        return {*this, end_tag{}};
      }
      CONCEPT_REQUIRES(meta::and_c<(bool) Range<R1 const>(),
                                   (bool) Range<R2 const>()>::value)
      cursor<true> begin_cursor() const
      {
        return {*this, begin_tag{}};
      }
      CONCEPT_REQUIRES(meta::and_c<(bool) Range<R1 const>(),
                                   (bool) Range<R2 const>()>::value)
      meta::if_<are_bounded_t, cursor<true>, sentinel<true>>
      end_cursor() const
      {
        return {*this, end_tag{}};
      }
    public:
      iter_series_mult_view() = default;
      explicit iter_series_mult_view(R1 r1, R2 r2)
        : r1_{std::move(r1)}
        , r2_{std::move(r2)}
      {}
      CONCEPT_REQUIRES(meta::and_c<(bool) SizedRange<R1>(),
                                   (bool) SizedRange<R2>()>::value)
      constexpr size_type_ size() const
      {
        return detail::series_mult_cardinality<
          range_cardinality<R1>, range_cardinality<R2>>::value;
      }
    };

    template<typename R1, typename R2>
    struct series_mult_view
      : iter_series_mult_view<R1, R2>
    {
      series_mult_view() = default;
      explicit series_mult_view(R1 r1, R2 r2)
        : iter_series_mult_view<R1, R2>{std::move(r1), std::move(r2)}
      {}
    };

    namespace view
    {
      struct iter_series_mult_fn
      {
        template<typename R1, typename R2>
        using Concept = meta::and_<
          BidirectionalRange<R1>, BidirectionalRange<R2>>;

        template<typename R1, typename R2,
                 CONCEPT_REQUIRES_(Concept<R1, R2>())>
        iter_series_mult_view<all_t<R1>, all_t<R2>> operator()(
            R1 && r1, R2 && r2) const
        {
          return iter_series_mult_view<all_t<R1>, all_t<R2>>{
              all(std::forward<R1>(r1)),
              all(std::forward<R2>(r2)),
          };
        }

#ifndef RANGES_DOXYGEN_INVOKED
        template<typename R1, typename R2,
                 CONCEPT_REQUIRES_(!Concept<R1, R2>())>
        void operator()(R1 && r1, R2 && r2) const
        {
          CONCEPT_ASSERT_MSG(meta::and_<BidirectionalRange<R1>, BidirectionalRange<R2>>(),
                             "All of the objects passed to view::iter_series_mult must model "
                             "the BidirectionalRange concept");
        }
#endif
      };

      namespace
      {
        constexpr auto&& iter_series_mult = static_const<iter_series_mult_fn>::value;
      }

      struct series_mult_fn
      {
        template<typename R1, typename R2>
        using Concept = meta::and_<
          BidirectionalRange<R1>, BidirectionalRange<R2>>;

        template<typename R1, typename R2,
                 CONCEPT_REQUIRES_(Concept<R1, R2>())>
        series_mult_view<all_t<R1>, all_t<R2>> operator()(
            R1 && r1, R2 && r2) const
        {
          return series_mult_view<all_t<R1>, all_t<R2>>{
              all(std::forward<R1>(r1)),
              all(std::forward<R2>(r2)),
          };
        }

#ifndef RANGES_DOXYGEN_INVOKED
        template<typename R1, typename R2,
                 CONCEPT_REQUIRES_(!Concept<R1, R2>())>
        void operator()(R1 &&, R2 &&) const
        {
          CONCEPT_ASSERT_MSG(meta::and_<BidirectionalRange<R1>, BidirectionalRange<R2>>(),
                             "All of the objects passed to view::series_mult must model "
                             "the BidirectionalRange concept");
        }
#endif
      };

      namespace
      {
        constexpr auto&& series_mult = static_const<with_braced_init_args<series_mult_fn>>::value;
      }

    } // namespace view
  }  // inline namespace v3
} // namespace ranges
