#ifndef __SDVIZ_SERDES_HPP__
# define __SDVIZ_SERDES_HPP__

# include <string>
# include <stdexcept>

# include <json11.hpp>
# include <server_ws.hpp>

# include "./action.hpp"
# include "./image_impl.hpp"
# include "./canvas_impl.hpp"
# include "./layout_impl.hpp"
# include "./element_impl.hpp"
# include "./variant_util.hpp"

namespace sdviz
{
    using serialized_type = std::string;
    using intermediate_array_type = json11::Json::array;
    using intermediate_map_type = json11::Json::object;
    using intermediate_type = json11::Json;

    bool isValid( intermediate_type const& _intermediate );
    serialized_type serialize( intermediate_type const& _intermediate );
    intermediate_type deserialize( serialized_type const& _serialize );

    template< typename T > struct ValueConvertedTypeTraits { using type = T; };
    template<> struct ValueConvertedTypeTraits< ImageImpl > { using type = intermediate_type; };
    template<> struct ValueConvertedTypeTraits< CanvasImpl > { using type = intermediate_type; };
    template<> struct ValueConvertedTypeTraits< LayoutImpl > { using type = intermediate_type; };

    template< typename T >
    inline typename ValueConvertedTypeTraits< T >::type valueToIntermediateType( T const& _value )
    {
        return _value;
    }

    template<>
    inline typename ValueConvertedTypeTraits< ImageImpl >::type valueToIntermediateType<ImageImpl>( ImageImpl const& _image )
    {
        auto image_size = ImageImpl::GetBufferSize( _image );
        std::string const image_str{ _image.getBuffer(), _image.getBuffer() + image_size };
        std::string const image_base64 = SimpleWeb::Crypto::Base64::encode( image_str );

        return intermediate_map_type{
            { "buffer", image_base64 },
            { "width", _image.getWidth() },
            { "height", _image.getHeight() },
            { "format", _image.getFormat() }
        };
    }

    template< typename T, size_t... I >
    intermediate_array_type tupleToIntermediateArrayImpl( T&& _tuple, std::index_sequence<I...> )
    {
        return intermediate_array_type{ valueToIntermediateType( std::get<I>( _tuple ) )... };
    }

    template< typename T, typename Indices = std::make_index_sequence< std::tuple_size<T>::value > >
    intermediate_array_type tupleToIntermediateArray( T&& _tuple )
    {
        return tupleToIntermediateArrayImpl( std::forward<T>( _tuple ), Indices() );
    }

    template<>
    inline typename ValueConvertedTypeTraits< CanvasImpl >::type valueToIntermediateType<CanvasImpl>( CanvasImpl const& _canvas )
    {
        auto visitor = makeVariantVisitor< intermediate_type >( []( auto const& command ){
            return intermediate_map_type{
                { "func", command.func_name },
                { "args", tupleToIntermediateArray( command.getParam() ) }
            };
        });

        intermediate_array_type commands;
        std::transform( _canvas.cbegin(),
                        _canvas.cend(),
                        std::back_inserter( commands ),
                        [&]( auto const& command ){ return boost::apply_visitor( visitor, command ); });
        return intermediate_map_type{
            { "commands", commands },
            { "width", _canvas.getWidth() },
            { "height", _canvas.getHeight() },
        };
    }

    template<>
    inline typename ValueConvertedTypeTraits< LayoutImpl >::type valueToIntermediateType<LayoutImpl>( LayoutImpl const& _layout )
    {
        std::vector< intermediate_type > result;
        std::transform( std::begin( _layout ),
                        std::end( _layout ),
                        std::back_inserter( result ),
                        []( auto const& _element ){
                            return intermediate_map_type{
                                { "span", std::get<0>( _element ) },
                                { "id", std::get<1>( _element ) }
                            };
        });

        return result;
    }

    template< typename ParamType >
    inline intermediate_type paramToIntermediateType( ParamType const& )
    {
        return intermediate_map_type{};
    }

    template<>
    inline intermediate_type paramToIntermediateType< ContainerElementImplParam >( ContainerElementImplParam const& _param )
    {
        return intermediate_map_type{
            { "label", _param.label },
            { "is_row_direction", _param.is_row_direction },
        };
    }

    template<>
    inline intermediate_type paramToIntermediateType< ButtonElementImplParam >( ButtonElementImplParam const& _param )
    {
        return intermediate_map_type{
            { "label", _param.label },
        };
    }

    template<>
    inline intermediate_type paramToIntermediateType< SliderElementImplParam >( SliderElementImplParam const& _param )
    {
        return intermediate_map_type{
            { "label", _param.label },
        };
    }

    template<>
    inline intermediate_type paramToIntermediateType< ChartElementImplParam >( ChartElementImplParam const& _param )
    {
        return intermediate_map_type{
            { "type", _param.type },
            { "value_map", _param.value_map }
        };
    }

    template< typename ElementImplType >
    inline intermediate_type elementImplToIntermediateType( std::string const& _target_id, ElementImplType const& _element )
    {
        auto const& value = _element.getValue();
        auto const intermediate_value = valueToIntermediateType( value );

        auto const& param = _element.getParam();
        auto const intermediate_param = paramToIntermediateType( param );

        int const version = _element.getVersion();

        int const type_index = GetVariantTypeIndex< ElementImplVariant, ElementImplType >::value;

        return intermediate_map_type {
            { "id", _target_id },
            { "type", type_index },
            { "value", intermediate_value },
            { "param", intermediate_param },
            { "version", version }
        };
    }

    inline intermediate_type elementImplToIntermediateType( std::string const& _target_id, ElementImplVariant const& _element_impl_variant )
    {
        auto visitor = makeVariantVisitor< intermediate_type >([&_target_id]( auto const& _element_impl ){
            return elementImplToIntermediateType( _target_id, _element_impl );
        });

        return boost::apply_visitor( visitor, _element_impl_variant );
    }

    ActionVariant intermediateTypeToSetValueAction( intermediate_type const& _intermediate_action );
}

#endif // __SDVIZ_SERDES_HPP__
