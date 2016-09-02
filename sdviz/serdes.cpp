#include "./serdes.hpp"

using namespace sdviz;

namespace
{
    bool isValidIntermediateObject( intermediate_map_type const& _obj )
    {
        return ( 0 < _obj.count("type") )
            && ( 0 < _obj.count("value") )
            && ( 0 < _obj.count("id") )
            && _obj.at("type").is_uint8();
    }
}

bool sdviz::isValid( intermediate_type const& _intermediate )
{
    return !_intermediate.is_null();
}

serialized_type sdviz::serialize( intermediate_type const& _intermediate )
{
    return _intermediate.dump();
}

intermediate_type sdviz::deserialize( serialized_type const& _serialize )
{
    std::string err_message;
    auto msgpack = msgpack11::MsgPack::parse( _serialize, err_message );
    if( 0 < err_message.length() )
    {
        throw std::runtime_error( "MsgPack parse failed." );
    }

    return msgpack;
}

ActionVariant sdviz::intermediateTypeToSetValueAction( intermediate_type const& _intermediate_action )
{
    if( !_intermediate_action.is_object() )
    {
        throw std::runtime_error( "Intermediate object has invalid format." );
    }

    intermediate_map_type obj = _intermediate_action.object_items();
    if( !isValidIntermediateObject( obj ) )
    {
        throw std::runtime_error( "Intermediate object has invalid format." );
    }

    std::string const target_id = obj["id"].string_value();
    int const type_index = obj["type"].int_value();
    switch (type_index) {
        case GetVariantTypeIndex< ElementImplVariant, ButtonElementImpl >::value:
            return ActionVariant{ ActionTypeTraits< ButtonElementImpl >::set_value_type{ target_id, obj["value"].bool_value() } };
        case GetVariantTypeIndex< ElementImplVariant, SliderElementImpl >::value:
            return ActionVariant{ ActionTypeTraits< SliderElementImpl >::set_value_type{ target_id, obj["value"].number_value() } };
    }

    throw std::runtime_error( "Intermediate object has invalid type." );
}
