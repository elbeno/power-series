#pragma once

#include <range/v3/view/zip_with.hpp>

namespace ranges
{
  inline namespace v3
  {
    namespace detail
    {
      template<typename C1, typename C2>
      using monoidal_zip_cardinality =
        std::integral_constant<cardinality,
          C1::value == infinite || C2::value == infinite ?
            infinite :
            C1::value == unknown || C2::value == unknown ?
              unknown :
              C1::value >= 0 && C2::value >= 0 ?
                max_(C1::value, C2::value) :
                finite>;
    } // namespace detail

    template<typename Fun, typename R1, typename R2>
    struct iter_monoidal_zip_view
      : view_facade<iter_monoidal_zip_view<Fun, R1, R2>,
                    detail::monoidal_zip_cardinality<range_cardinality<R1>,
                                                     range_cardinality<R2>>::value>
    {
    private:
      friend range_access;
      semiregular_t<function_type<Fun>> fun_;
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
        using monoidal_zip_view_t = constify_if<iter_monoidal_zip_view>;
        monoidal_zip_view_t *rng_;
        using fun_ref_ = semiregular_ref_or_val_t<function_type<Fun>, true>;
        fun_ref_ fun_;
        range_iterator_t<constify_if<R1>> it1_;
        range_iterator_t<constify_if<R2>> it2_;
        difference_type diff_;

      public:
        using single_pass = meta::fast_or<SinglePass<range_iterator_t<R1>>,
                                          SinglePass<range_iterator_t<R2>>>;

        cursor() = default;
        cursor(monoidal_zip_view_t &rng, fun_ref_ fun, begin_tag)
          : rng_{&rng}
          , fun_{std::move(fun)}
          , it1_{begin(rng.r1_)}
          , it2_{begin(rng.r2_)}
          , diff_{0}
        {}
        cursor(monoidal_zip_view_t &rng, fun_ref_ fun, end_tag)
          : rng_{&rng}
          , fun_{std::move(fun)}
          , it1_{end(rng.r1_)}
          , it2_{end(rng.r2_)}
          , diff_{end_diff()}
        {}
        difference_type end_diff()
        {
          difference_type d = detail::distance_to(begin(rng_->r1_), it1_) -
            detail::distance_to(begin(rng_->r2_), it2_);
          return d > 0 ? d + 1 : d - 1;
        }
        auto current() const
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
          diff_ == 0 ?
            fun_(it1_, it2_) :
            diff_ > 0 ? *it1_ : *it2_
        )
        void next()
        {
          // r1 longer than r2
          if (diff_ > 0)
          {
            ++it1_;
            ++diff_;
            return;
          }
          // r2 longer than r1
          if (diff_ < 0)
          {
            ++it2_;
            --diff_;
            return;
          }
          // same
          if (++it2_ == end(rng_->r2_))
            ++diff_;
          if (++it1_ == end(rng_->r1_))
            --diff_;
        }
        bool equal(cursor const &that) const
        {
          return it1_ == that.it1_
            && it2_ == that.it2_;
        }
        CONCEPT_REQUIRES(meta::and_c<(bool) BidirectionalRange<R1>(),
                                     (bool) BidirectionalRange<R2>()>::value)
        void prev()
        {
          // r1 longer than r2
          if (diff_ > 0)
          {
            --it1_;
            if (--diff_ == 0)
              --it2_;
            return;
          }
          // r2 longer than r1
          if (diff_ < 0)
          {
            --it2_;
            if (++diff_ == 0)
              --it1_;
            return;
          }
          // same
          --it1_;
          --it2_;
        }
        CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessRange<R1>(),
                                     (bool) RandomAccessRange<R2>()>::value)
        difference_type distance_to(cursor const &that) const
        {
          return detail::max_(detail::distance_to(it1_, that.it1_),
                              detail::distance_to(it2_, that.it2_));
        }
      };

      template <bool IsConst>
      struct sentinel
      {
      private:
        template <typename T>
        using constify_if = meta::apply<meta::add_const_if_c<IsConst>, T>;
        using monoidal_zip_view_t = constify_if<iter_monoidal_zip_view>;
        using fun_ref_ = semiregular_ref_or_val_t<function_type<Fun>, true>;
        range_sentinel_t<constify_if<R1>> end1_;
        range_sentinel_t<constify_if<R2>> end2_;
      public:
        sentinel() = default;
        sentinel(monoidal_zip_view_t &rng, fun_ref_, end_tag)
          : end1_(end(rng.r1_))
          , end2_(end(rng.r2_))
        {}
        bool equal(cursor<IsConst> const &pos) const
        {
          return detail::equal_to(pos.it1_, end1_)
            && detail::equal_to(pos.it2_, end2_);
        }
      };

      using are_bounded_t = meta::and_c<(bool) BoundedRange<R1>(),
                                        (bool) BoundedRange<R2>()>;

      cursor<false> begin_cursor()
      {
        return {*this, fun_, begin_tag{}};
      }
      meta::if_<are_bounded_t, cursor<false>, sentinel<false>>
      end_cursor()
      {
        return {*this, fun_, end_tag{}};
      }
      CONCEPT_REQUIRES(meta::and_c<(bool) Range<R1 const>(),
                                   (bool) Range<R2 const>()>::value)
      cursor<true> begin_cursor() const
      {
        return {*this, fun_, begin_tag{}};
      }
      CONCEPT_REQUIRES(meta::and_c<(bool) Range<R1 const>(),
                                   (bool) Range<R2 const>()>::value)
      meta::if_<are_bounded_t, cursor<true>, sentinel<true>>
      end_cursor() const
      {
        return {*this, fun_, end_tag{}};
      }
    public:
      iter_monoidal_zip_view() = default;
      explicit iter_monoidal_zip_view(R1 r1, R2 r2)
        : fun_(as_function(Fun{}))
        , r1_{std::move(r1)}
        , r2_{std::move(r2)}
      {}
      explicit iter_monoidal_zip_view(Fun fun, R1 r1, R2 r2)
        : fun_(as_function(std::move(fun)))
        , r1_{std::move(r1)}
        , r2_{std::move(r2)}
      {}
      CONCEPT_REQUIRES(meta::and_c<(bool) SizedRange<R1>(),
                                   (bool) SizedRange<R2>()>::value)
      constexpr size_type_ size() const
      {
        return range_cardinality<R1>::value > 0 && range_cardinality<R1>::value > 0 ?
          static_cast<size_type_>(detail::max_(range_cardinality<R1>::value,
                                               range_cardinality<R2>::value)) :
          detail::max_(ranges::size(r1_), ranges::size(r2_));
      }
    };

    template<typename Fun, typename R1, typename R2>
    struct monoidal_zip_view
      : iter_monoidal_zip_view<indirected<Fun>, R1, R2>
    {
      monoidal_zip_view() = default;
      explicit monoidal_zip_view(R1 r1, R2 r2)
        : iter_monoidal_zip_view<indirected<Fun>, R1, R2>{
        {Fun{}}, std::move(r1), std::move(r2)}
      {}
      explicit monoidal_zip_view(Fun fun, R1 r1, R2 r2)
        : iter_monoidal_zip_view<indirected<Fun>, R1, R2>{
        {std::move(fun)}, std::move(r1), std::move(r2)}
      {}
    };

    namespace view
    {
      struct iter_monoidal_zip_fn
      {
        template<typename Fun, typename R1, typename R2>
        using Concept = meta::and_<
          InputRange<R1>, InputRange<R2>,
          Callable<Fun, range_iterator_t<R1>, range_iterator_t<R2>>,
          Callable<Fun, copy_tag, range_iterator_t<R1>, range_iterator_t<R2>>,
          Callable<Fun, move_tag, range_iterator_t<R1>, range_iterator_t<R2>>>;

        template<typename R1, typename R2, typename Fun,
                 CONCEPT_REQUIRES_(Concept<Fun, R1, R2>())>
        iter_monoidal_zip_view<Fun, all_t<R1>, all_t<R2>> operator()(
            Fun fun, R1 && r1, R2 && r2) const
        {
          return iter_monoidal_zip_view<Fun, all_t<R1>, all_t<R2>>{
              std::move(fun),
              all(std::forward<R1>(r1)),
              all(std::forward<R2>(r2))
          };
        }

#ifndef RANGES_DOXYGEN_INVOKED
        template<typename Fun, typename R1, typename R2,
                 CONCEPT_REQUIRES_(!Concept<Fun, R1, R2>())>
        void operator()(Fun, R1 &&, R2 &&) const
        {
          CONCEPT_ASSERT_MSG(meta::and_<InputRange<R1>, InputRange<R2>>(),
                             "All of the objects passed to view::iter_monoidal_zip must model "
                             "the InputRange concept");
          CONCEPT_ASSERT_MSG(
              Callable<Fun, range_iterator_t<R1>, range_iterator_t<R2>>(),
              "The function passed to view::iter_monoidal_zip must be callable with arguments "
              "of the ranges' iterator types.");
          CONCEPT_ASSERT_MSG(
              Callable<Fun, copy_tag, range_iterator_t<R1>, range_iterator_t<R2>>(),
              "The function passed to view::iter_monoidal_zip must be callable with "
              "copy_tag and arguments of the ranges' iterator types.");
          CONCEPT_ASSERT_MSG(
              Callable<Fun, move_tag, range_iterator_t<R1>, range_iterator_t<R2>>(),
              "The function passed to view::iter_monoidal_zip must be callable with "
              "move_tag and arguments of the ranges' iterator types.");
        }
#endif
      };

      namespace
      {
        constexpr auto&& iter_monoidal_zip = static_const<iter_monoidal_zip_fn>::value;
      }

      struct monoidal_zip_fn
      {
        template<typename Fun, typename R1, typename R2>
        using Concept = meta::and_<
          InputRange<R1>, InputRange<R2>,
          Callable<Fun, range_reference_t<R1> &&, range_reference_t<R2> &&>>;

        template<typename R1, typename R2, typename Fun,
                 CONCEPT_REQUIRES_(Concept<Fun, R1, R2>())>
        monoidal_zip_view<Fun, all_t<R1>, all_t<R2>> operator()(
            Fun fun, R1 && r1, R2 && r2) const
        {
          return monoidal_zip_view<Fun, all_t<R1>, all_t<R2>>{
              std::move(fun),
              all(std::forward<R1>(r1)),
              all(std::forward<R2>(r2))
          };
        }

#ifndef RANGES_DOXYGEN_INVOKED
        template<typename Fun, typename R1, typename R2,
                 CONCEPT_REQUIRES_(!Concept<Fun, R1, R2>())>
        void operator()(Fun, R1 &&, R2 &&) const
        {
          CONCEPT_ASSERT_MSG(meta::and_<InputRange<R1>, InputRange<R2>>(),
                             "All of the objects passed to view::monoidal_zip must model "
                             "the InputRange concept");
          CONCEPT_ASSERT_MSG(
              Callable<Fun, range_reference_t<R1> &&, range_reference_t<R2> &&>(),
              "The function passed to view::monoidal_zip must be callable with arguments "
              "of the ranges' reference types.");
        }
#endif
      };

      namespace
      {
        constexpr auto&& monoidal_zip = static_const<monoidal_zip_fn>::value;
      }

    } // namespace view
  }  // inline namespace v3
} // namespace ranges
