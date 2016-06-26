#ifndef __SDVIZ_VARIANT_UTIL_HPP__
# define __SDVIZ_VARIANT_UTIL_HPP__

# include <boost/variant.hpp>
# include <boost/mpl/find.hpp>
# include <boost/mpl/distance.hpp>

namespace sdviz
{
    template < typename R, typename F>
    class VariantVisitor final {
        public:
            using result_type = R;

            VariantVisitor(F f) : f_(f) {}

            template < typename... ArgTypes >
            result_type operator()(ArgTypes&... _args)
            {
                return f_(_args...);
            }

            template < typename... ArgTypes >
            result_type operator()(ArgTypes const&... _args)
            {
                return f_(_args...);
            }

            ~VariantVisitor() = default;
            VariantVisitor( VariantVisitor const& ) = default;
            VariantVisitor( VariantVisitor&& ) = default;
            VariantVisitor& operator =( VariantVisitor const& ) = default;
            VariantVisitor& operator =( VariantVisitor&& ) = default;

        private:
            F f_;
    };

    template < typename R, typename F >
    VariantVisitor<R, F> makeVariantVisitor(F f)
    {
        return VariantVisitor<R, F>{f};
    };

    template < typename VariantType, typename T >
    struct GetVariantTypeIndex
    {
        private:
            using TypeIndexIter = typename boost::mpl::find< typename VariantType::types, T >::type;
            using TypeIndexFirst = typename boost::mpl::begin< typename VariantType::types >::type;

        public:
            enum { value = boost::mpl::distance< TypeIndexFirst, TypeIndexIter >::value };
    };
}

#endif // __SDVIZ_VARIANT_UTIL_HPP__
