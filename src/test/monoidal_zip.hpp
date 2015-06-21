#pragma once

#include <range/v3/view/concat.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/zip_with.hpp>

#include <type_traits>

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

      template<typename R2, typename Z>
      using monoidal_zip_extended_range =
        meta::if_c<range_cardinality<R2>::value == infinite,
                   R2,
                   concat_view<R2, repeat_view<Z>>>;

      template<typename R2, typename Z>
      monoidal_zip_extended_range<R2, Z>
      make_extended_range(R2&& r2, Z&&,
                          std::enable_if_t<range_cardinality<R2>::value == infinite>* = 0)
      {
        return r2;
      }

      template<typename R2, typename Z>
      monoidal_zip_extended_range<R2, Z>
      make_extended_range(R2&& r2, Z&& z,
                          std::enable_if_t<range_cardinality<R2>::value != infinite>* = 0)
      {
        return view::concat(std::forward<R2>(r2),
                            view::repeat(std::forward<Z>(z)));
      }

    } // namespace detail

    template<typename Fun, typename R1, typename R2_, typename Z>
    struct iter_monoidal_zip_view
      : view_facade<iter_monoidal_zip_view<Fun, R1, R2_, Z>,
                    range_cardinality<R1>::value>
    {
    private:
      friend range_access;
      semiregular_t<function_type<Fun>> fun_;

      using R2 = detail::monoidal_zip_extended_range<R2_, Z>;
      R1 r1_;
      R2 r2_;
      using difference_type_ = common_type_t<range_difference_t<R1>,
                                             range_difference_t<R2>>;
      using size_type_ = meta::eval<std::make_unsigned<difference_type_>>;

      struct sentinel;
      struct cursor
      {
      private:
        friend sentinel;
        using fun_ref_ = semiregular_ref_or_val_t<function_type<Fun>, true>;
        fun_ref_ fun_;
        range_iterator_t<R1> it1_;
        range_iterator_t<R2> it2_;

        auto indirect_move_() const
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
          fun_(move_tag{}, it1_, it2_)
        )
        template<typename Sent>
        friend auto indirect_move(basic_iterator<cursor, Sent> const &it)
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            get_cursor(it).indirect_move_()
        )
      public:
        using difference_type = common_type_t<range_difference_t<R1>,
                                              range_difference_t<R2>>;
        using single_pass =
          meta::or_c<(bool) Derived<ranges::input_iterator_tag,
                                    range_category_t<R1>>(),
                     (bool) Derived<ranges::input_iterator_tag,
                                    range_category_t<R2>>()>;
        using value_type =
          detail::decay_t<decltype(fun_(copy_tag{},
                                        range_iterator_t<R1>{},
                                        range_iterator_t<R2>{}))>;

        cursor() = default;
        cursor(fun_ref_ fun, range_iterator_t<R1> it1, range_iterator_t<R2> it2)
          : fun_(std::move(fun)), it1_(std::move(it1)), it2_(std::move(it2))
        {}
        auto current() const
        RANGES_DECLTYPE_AUTO_RETURN_NOEXCEPT
        (
            fun_(it1_, it2_)
        )
        void next()
        {
          detail::inc(it1_);
          detail::inc(it2_);
        }
        bool equal(cursor const &that) const
        {
          return detail::equal_to(it1_, that.it1_);
        }
        CONCEPT_REQUIRES(meta::and_c<(bool) BidirectionalRange<R1>(),
                                     (bool) BidirectionalRange<R2>()>::value)
        void prev()
        {
          detail::dec(it1_);
          detail::dec(it2_);
        }
        CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessRange<R1>(),
                                     (bool) RandomAccessRange<R2>()>::value)
        void advance(difference_type n)
        {
          detail::advance_(it1_, n);
          detail::advance_(it2_, n);
        }
        CONCEPT_REQUIRES(meta::and_c<(bool) RandomAccessRange<R1>(),
                                     (bool) RandomAccessRange<R2>()>::value)
        difference_type distance_to(cursor const &that) const
        {
          return detail::distance_to(it1_, that.it1_);
        }
      };

      struct sentinel
      {
      private:
        range_sentinel_t<R1> end1_;
        range_sentinel_t<R2> end2_;
      public:
        sentinel() = default;
        sentinel(detail::any, range_sentinel_t<R1> end1, range_sentinel_t<R2> end2)
          : end1_(std::move(end1)), end2_(std::move(end2))
        {}
        bool equal(cursor const &pos) const
        {
          return detail::equal_to(pos.it1_, end1_);
        }
      };

      using are_bounded_t = meta::and_c<(bool) BoundedRange<R1>()>;

      cursor begin_cursor()
      {
        return {fun_, begin(r1_), begin(r2_)};
      }
      meta::if_<are_bounded_t, cursor, sentinel> end_cursor()
      {
        return {fun_, end(r1_), begin(r2_)};
      }
      CONCEPT_REQUIRES(meta::and_c<(bool) Range<R1 const>(),
                                   (bool) Range<R2 const>()>::value)
      cursor begin_cursor() const
      {
        return {fun_, begin(r1_), begin(r2_)};
      }
      CONCEPT_REQUIRES(meta::and_c<(bool) Range<R1 const>(),
                                   (bool) Range<R2 const>()>::value)
      meta::if_<are_bounded_t, cursor, sentinel> end_cursor() const
      {
        return {fun_, end(r1_), begin(r2_)};
      }
    public:
      iter_monoidal_zip_view() = default;
      explicit iter_monoidal_zip_view(R1 r1, R2_ r2, Z z)
        : fun_(as_function(Fun{}))
        , r1_{std::move(r1)}
        , r2_{detail::make_extended_range(std::move(r2), std::move(z))}
      {}
      explicit iter_monoidal_zip_view(Fun fun, R1 r1, R2_ r2, Z z)
        : fun_(as_function(std::move(fun)))
        , r1_{std::move(r1)}
        , r2_{detail::make_extended_range(std::move(r2), std::move(z))}
      {}
      CONCEPT_REQUIRES((bool) SizedRange<R1>())
      constexpr size_type_ size() const
      {
        (size_type_)range_cardinality<R1>::value;
      }
    };

    template<typename Fun, typename R1, typename R2, typename Z>
    struct monoidal_zip_view
      : iter_monoidal_zip_view<indirected<Fun>, R1, R2, Z>
    {
      monoidal_zip_view() = default;
      explicit monoidal_zip_view(R1 r1, R2 r2, Z z)
        : iter_monoidal_zip_view<indirected<Fun>, R1, R2, Z>{
        {Fun{}}, std::move(r1), std::move(r2), std::move(z)}
      {}
      explicit monoidal_zip_view(Fun fun, R1 r1, R2 r2, Z z)
        : iter_monoidal_zip_view<indirected<Fun>, R1, R2, Z>{
        {std::move(fun)}, std::move(r1), std::move(r2), std::move(z)}
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

        template<typename R1, typename R2, typename Z, typename Fun,
                 CONCEPT_REQUIRES_(Concept<Fun, R1, R2>())>
        iter_monoidal_zip_view<Fun, all_t<R1>, all_t<R2>, Z> operator()(
            Fun fun, R1 && r1, R2 && r2, Z && z) const
        {
          return iter_monoidal_zip_view<Fun, all_t<R1>, all_t<R2>, Z>{
              std::move(fun),
              all(std::forward<R1>(r1)),
              all(std::forward<R2>(r2)),
              std::forward<Z>(z)
          };
        }

#ifndef RANGES_DOXYGEN_INVOKED
        template<typename Fun, typename R1, typename R2,
                 CONCEPT_REQUIRES_(!Concept<Fun, R1, R2>())>
        void operator()(Fun, R1 && r1, R2 && r2) const
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

        template<typename R1, typename R2, typename Z, typename Fun,
                 CONCEPT_REQUIRES_(Concept<Fun, R1, R2>())>
        monoidal_zip_view<Fun, all_t<R1>, all_t<R2>, Z> operator()(
            Fun fun, R1 && r1, R2 && r2, Z && z) const
        {
          return monoidal_zip_view<Fun, all_t<R1>, all_t<R2>, Z>{
              std::move(fun),
              all(std::forward<R1>(r1)),
              all(std::forward<R2>(r2)),
              std::forward<Z>(z)
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
