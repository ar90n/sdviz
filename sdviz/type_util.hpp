#ifndef __SDVIZ_TYPE_UTIL_HPP__
# define __SDVIZ_TYPE_UTIL_HPP__

# include <type_traits>

# include "sdviz.hpp"
# include "image_impl.hpp"
# include "canvas_impl.hpp"
# include "element_impl.hpp"

namespace sdviz
{
    Image::Format convertToImageFormat( ImageImpl::Format const _format );
    ImageImpl::Format convertToImageImplFormat( Image::Format const _format );
    std::string convertToChartImplType( ChartElementParam::Type const _type );

    template< typename T > struct ImplTypeTraits {};
    template<> struct ImplTypeTraits< TextElement > { using type = TextElementImpl; };
    template<> struct ImplTypeTraits< CanvasElement > { using type = CanvasElementImpl; };
    template<> struct ImplTypeTraits< ChartElement > { using type = ChartElementImpl; };
    template<> struct ImplTypeTraits< ButtonElement > { using type = ButtonElementImpl; };
    template<> struct ImplTypeTraits< SliderElement > { using type = SliderElementImpl; };

    template< typename ParamType >
    struct HasOnValueChanged
    {
        template< typename T >
        static std::true_type test( decltype( T::on_value_changed )* )
        {
            return std::true_type();
        }

        template< typename T >
        static std::false_type test( ... )
        {
            return std::false_type();
        }

        using type = decltype(test< ParamType >(0));
        static constexpr bool value = type::value;
    };

    template< typename WrapType >
    inline typename ImplTypeTraits< WrapType >::type::value_type convertToImplValue( typename WrapType::value_type const& _value )
    {
        using impl_value_type = typename ImplTypeTraits< WrapType >::type::value_type;
        return impl_value_type{ _value };
    }

    template<>
    inline typename ImplTypeTraits< CanvasElement >::type::value_type  convertToImplValue< CanvasElement >( CanvasElement::value_type const& _canvas )
    {
        using impl_value_type = typename ImplTypeTraits< CanvasElement >::type::value_type;
        return impl_value_type{ CanvasImpl{ *_canvas.getImpl() } };
    }

    template< typename WrapType,
              typename std::enable_if_t<
                  !HasOnValueChanged< typename ImplTypeTraits< WrapType >::type::param_type >::value,
                  std::nullptr_t
              > = nullptr >
    inline typename ImplTypeTraits< WrapType >::type::param_type convertToImplParam( typename WrapType::param_type const&)
    {
        using impl_param_type = typename ImplTypeTraits< WrapType >::type::param_type;
        return impl_param_type{};
    }

    template< typename WrapType,
              typename std::enable_if_t<
                  HasOnValueChanged< typename ImplTypeTraits< WrapType >::type::param_type >::value,
                  std::nullptr_t
              > = nullptr >
    inline typename ImplTypeTraits< WrapType >::type::param_type convertToImplParam( typename WrapType::param_type const& _param)
    {
        using impl_param_type = typename ImplTypeTraits< WrapType >::type::param_type;
        return impl_param_type{
            _param.label,
            _param.on_value_changed
        };
    }

    template<>
    inline typename ImplTypeTraits< ChartElement >::type::param_type convertToImplParam< ChartElement >( typename ChartElement::param_type const& _param)
    {
        using impl_param_type = typename ImplTypeTraits< ChartElement >::type::param_type;
        return impl_param_type{
            convertToChartImplType( _param.type ),
            _param.value_map
        };
    }

}

#endif // __SDVIZ_TYPE_UTIL_HPP__
