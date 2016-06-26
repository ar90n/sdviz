#ifndef __SDVIZ_ELEMENT_IMPL_HPP__
# define __SDVIZ_ELEMENT_IMPL_HPP__

# include <string>
# include <map>
# include <vector>

# include <boost/variant.hpp>

# include "./layout_impl.hpp"
# include "./canvas_impl.hpp"

namespace sdviz
{
    template< typename ValueType, typename ParamType >
    class ElementImpl final
    {
        public:
            using value_type = ValueType;
            using param_type = ParamType;

            ElementImpl() = delete;
            ElementImpl( value_type&& _value, param_type const& _param )
                : value( std::forward< value_type >( _value ) ),
                  param( _param ),
                  version( 0 )
            {
            }

            ElementImpl( ElementImpl const& _element ) = delete;
            ElementImpl( ElementImpl&& _element ) = default;
            ~ElementImpl() = default;

            ElementImpl& operator =( ElementImpl const& _element ) = delete;
            ElementImpl& operator =( ElementImpl&& _element ) = default;

            value_type const& getValue() const
            {
                return value;
            }

            void setValue( value_type&& _value )
            {
                value = std::forward< value_type >( _value );
                version++;
            }

            param_type const& getParam() const
            {
                return param;
            }

            void setParam( param_type&& _param )
            {
                param = std::forward< param_type >( _param );
                version++;
            }

            int getVersion() const
            {
                return version;
            }

        private:
            value_type value;
            param_type param;
            int version;
    };

    struct ButtonElementImplParam
    {
        std::string label;
        std::function< void( bool, bool ) > on_value_changed;
    };
    using ButtonElementImpl = ElementImpl< bool, ButtonElementImplParam >;

    struct CanvasElementImplParam
    {
    };
    using CanvasElementImpl = ElementImpl< CanvasImpl, CanvasElementImplParam >;

    struct ChartElementImplParam
    {
        std::string type;
        std::map< std::string, std::string > value_map;
    };
    using ChartElementImpl = ElementImpl< std::map< std::string, std::vector< double > >, ChartElementImplParam >;

    struct ContainerElementImplParam
    {
        std::string label;
        bool is_row_direction;
    };
    using ContainerElementImpl = ElementImpl< LayoutImpl, ContainerElementImplParam >;

    struct SliderElementImplParam
    {
        std::string label;
        std::function< void( double, double ) > on_value_changed;
    };
    using SliderElementImpl = ElementImpl< double, SliderElementImplParam >;

    struct TextElementImplParam
    {
    };
    using TextElementImpl = ElementImpl< std::string, TextElementImplParam >;

    using ElementImplVariant = boost::variant<
        TextElementImpl,
        CanvasElementImpl,
        ChartElementImpl,
        ContainerElementImpl,
        ButtonElementImpl,
        SliderElementImpl
    >;
}

#endif //__SDVIZ_ELEMENT_IMPL_HPP__
