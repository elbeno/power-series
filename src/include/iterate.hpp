#pragma once

#include <range/v3/view/generate.hpp>

namespace ranges
{
  inline namespace v3
  {
    template<typename G, typename T>
    struct iterate_view
      : view_facade<iterate_view<G, T>, infinite>
    {
    private:
      friend struct range_access;
      using result_t = concepts::Function::result_t<G, T>;
      semiregular_t<G> gen_;
      semiregular_t<result_t> val_;
      struct cursor
      {
      private:
        iterate_view *view_;
      public:
        using single_pass = std::true_type;
        cursor() = default;
        cursor(iterate_view &view)
          : view_(&view)
        {}
        constexpr bool done() const
        {
          return false;
        }
        result_t current() const
        {
          return view_->val_;
        }
        void next()
        {
          view_->next();
        }
      };
      void next()
      {
        val_ = gen_(val_);
      }
      cursor begin_cursor()
      {
        return {*this};
      }
    public:
      iterate_view() = default;
      explicit iterate_view(G g, T&& t)
        : gen_(std::move(g)), val_(std::move(t))
      {}
      result_t & cached()
      {
        return val_;
      }
    };

    namespace view
    {
      struct iterate_fn
      {
        template<typename G, typename T>
        using Concept = meta::and_<
          Function<G, T>,
          meta::not_<Same<void, concepts::Function::result_t<G, T>>>>;

        template<typename G, typename T,
                 CONCEPT_REQUIRES_(Concept<G, T>())>
        iterate_view<G, T> operator()(G g, T t) const
        {
          return iterate_view<G, T>{std::move(g), std::move(t)};
        }

#ifndef RANGES_DOXYGEN_INVOKED
        template<typename G, typename T,
                 CONCEPT_REQUIRES_(!Concept<G, T>())>
        void operator()(G, T) const
        {
          CONCEPT_ASSERT_MSG(
              Callable<G, T>(),
              "The function passed to view::iterate must be callable with the initial "
              "argument.");
          CONCEPT_ASSERT_MSG(
              Callable<G, concepts::Function::result_t<G, T>>(),
              "The function passed to view::iterate must be callable with its own "
              "result.");
        }
#endif
      };

      namespace
      {
        constexpr auto&& iterate = static_const<iterate_fn>::value;
      }
    }
  }
}
