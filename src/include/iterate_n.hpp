#pragma once

#include <range/v3/view/generate.hpp>

namespace ranges
{
  inline namespace v3
  {
    template<typename G, typename T>
    struct iterate_n_view
      : view_facade<iterate_n_view<G, T>, infinite>
    {
    private:
      friend struct range_access;
      using result_t = concepts::Function::result_t<G, T>;
      semiregular_t<G> gen_;
      semiregular_t<result_t> val_;
      std::size_t n_;
      struct cursor
      {
      private:
        iterate_n_view *view_;
        std::size_t n_;
      public:
        using single_pass = std::true_type;
        cursor() = default;
        cursor(iterate_n_view &view, std::size_t n)
          : view_(&view), n_(n)
        {}
        constexpr bool done() const
        {
          return 0 == n_;
        }
        result_t current() const
        {
          return view_->val_;
        }
        void next()
        {
          RANGES_ASSERT(0 != n_);
          if(0 != --n_)
            view_->next();
        }
      };
      void next()
      {
        val_ = gen_(val_);
      }
      cursor begin_cursor()
      {
        return {*this, n_};
      }
    public:
      iterate_n_view() = default;
      explicit iterate_n_view(G g, T&& t, std::size_t n)
        : gen_(std::move(g)), val_(std::move(t)), n_(n)
      {}
      result_t & cached()
      {
        return val_;
      }
      std::size_t size() const
      {
        return n_;
      }
    };

    namespace view
    {
      struct iterate_n_fn
      {
        template<typename G, typename T>
        using Concept = meta::and_<
          Function<G, T>,
          meta::not_<Same<void, concepts::Function::result_t<G, T>>>>;

        template<typename G, typename T,
                 CONCEPT_REQUIRES_(Concept<G, T>())>
        iterate_n_view<G, T> operator()(G g, T t, size_t n) const
        {
          return iterate_n_view<G, T>{std::move(g), std::move(t), n};
        }

#ifndef RANGES_DOXYGEN_INVOKED
        template<typename G, typename T,
                 CONCEPT_REQUIRES_(!Concept<G, T>())>
        void operator()(G, T, size_t) const
        {
          CONCEPT_ASSERT_MSG(
              Callable<G, T>(),
              "The function passed to view::iterate_n must be callable with the initial "
              "argument.");
          CONCEPT_ASSERT_MSG(
              Callable<G, concepts::Function::result_t<G, T>>(),
              "The function passed to view::iterate_n must be callable with its own "
              "result.");
        }
#endif
      };

      namespace
      {
        constexpr auto&& iterate_n = static_const<iterate_n_fn>::value;
      }
    }
  }
}
