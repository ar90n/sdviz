#ifndef __SDVIZ_ACTION_HPP__
# define __SDVIZ_ACTION_HPP__

# include <string>

# include "element_impl.hpp"

namespace sdviz
{
    template< typename PayloadType >
    class Action final
    {
        public:
            using payload_type = PayloadType;

            template< typename... ArgTypes >
            Action( std::string const& _target_id, ArgTypes&&... _args )
                : target_id{ _target_id },
                  payload( std::move( _args )... )
            {}
            Action( Action const& ) = delete;
            Action( Action&& ) = default;
            ~Action() = default;

            Action& operator =( Action const& ) = delete;
            Action& operator =( Action&& ) = default;

            std::string const target_id;
            payload_type  payload;
    };

    template< typename ElementImplType >
    struct ActionTypeTraits
    {
        using set_value_type = Action< typename ElementImplType::value_type >;
        using set_param_type = Action< typename ElementImplType::param_type >;
    };

    using AddElementImplAction = Action< std::tuple< int, std::string > >;
    using CreateElementImplAction = Action< ElementImplVariant >;
    using SyncAction = Action< std::nullptr_t >;
    using ActionVariant = boost::variant<
        ActionTypeTraits< TextElementImpl >::set_value_type,
        ActionTypeTraits< TextElementImpl >::set_param_type,
        ActionTypeTraits< CanvasElementImpl >::set_value_type,
        ActionTypeTraits< CanvasElementImpl >::set_param_type,
        ActionTypeTraits< ChartElementImpl >::set_value_type,
        ActionTypeTraits< ChartElementImpl >::set_param_type,
        ActionTypeTraits< ButtonElementImpl >::set_value_type,
        ActionTypeTraits< ButtonElementImpl >::set_param_type,
        ActionTypeTraits< SliderElementImpl >::set_value_type,
        ActionTypeTraits< SliderElementImpl >::set_param_type,
        AddElementImplAction,
        CreateElementImplAction,
        SyncAction
    >;
}

#endif // __SDVIZ_ACTION_HPP__
