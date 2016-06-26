#ifndef __SDVIZ_ACTION_DISPATCH_HPP__
# define __SDVIZ_ACTION_DISPATCH_HPP__

# include <string>
# include <sstream>

# include <boost/variant.hpp>

# include "./action.hpp"
# include "./resource.hpp"
# include "./serdes.hpp"
# include "./log.hpp"
# include "./type_util.hpp"

namespace sdviz
{
    struct ElementImplActionVisitor : public boost::static_visitor< intermediate_type >
    {
        template< typename ParamType,
                  typename ValueType,
                  typename std::enable_if_t<
                      HasOnValueChanged< ParamType >::value,
                      std::nullptr_t
                  > = nullptr >
        static void runOnValueChanged( ParamType const& _param, ValueType const& _current, ValueType const& _next )
        {
            _param.on_value_changed( _current, _next );
        }

        template< typename ParamType,
                  typename ValueType,
                  typename std::enable_if_t<
                      !HasOnValueChanged< ParamType >::value,
                      std::nullptr_t
                  > = nullptr >
        static void runOnValueChanged( ParamType const&, ValueType const&, ValueType const& )
        {
        }

        intermediate_type operator()( ContainerElementImpl& _element_impl, AddElementImplAction& _action ) const
        {
            int const span = std::get<0>( _action.payload );
            std::string const& id = std::get<1>( _action.payload );

            auto new_value{ _element_impl.getValue() };
            new_value.emplace_back( std::make_tuple( span, id ) );
            _element_impl.setValue( std::move( new_value ) );

            return elementImplToIntermediateType( _action.target_id, _element_impl );
        }

        template< typename ElementImplType,
                  typename ActionType,
                  typename std::enable_if_t<
                      std::is_same<
                          typename ActionType::payload_type,
                          typename ElementImplType::value_type
                      >::value,
                      std::nullptr_t
                  > = nullptr >
        intermediate_type operator()( ElementImplType& _element_impl, ActionType& _action ) const
        {
            runOnValueChanged( _element_impl.getParam(), _element_impl.getValue(), _action.payload );

            _element_impl.setValue( std::move( _action.payload ) );
            return elementImplToIntermediateType( _action.target_id, _element_impl );
        }

        template< typename ElementImplType,
                  typename ActionType,
                  typename std::enable_if_t<
                      std::is_same<
                          typename ActionType::payload_type,
                          typename ElementImplType::param_type
                      >::value,
                      std::nullptr_t
                  > = nullptr >
        intermediate_type operator()( ElementImplType& _element_impl, ActionType& _action ) const
        {
            _element_impl.setParam( std::move( _action.payload ) );
            return elementImplToIntermediateType( _action.target_id, _element_impl );
        }

        template< typename ElementImplType,
                  typename ActionType,
                  typename std::enable_if_t<
                      !std::is_same<
                          typename ActionType::payload_type,
                          typename ElementImplType::value_type
                      >::value &&
                      !std::is_same<
                          typename ActionType::payload_type,
                          typename ElementImplType::param_type
                      >::value,
                      std::nullptr_t
                  > = nullptr >
        intermediate_type operator()( ElementImplType&, ActionType& ) const
        {
            throw std::runtime_error( "Invalid ElementImplAction dispatch." );
        }
    };

    struct ActionVisitor : public boost::static_visitor< intermediate_type >
    {
        intermediate_type operator()( CreateElementImplAction& _action ) const
        {
            element_store.insert( std::make_pair( _action.target_id, std::move( _action.payload ) ) );

            std::map< std::string, intermediate_type > result;
            return elementImplToIntermediateType( _action.target_id, element_store.at( _action.target_id ) );
        }

        intermediate_type operator()( SyncAction& ) const
        {
            intermediate_array_type result;
            for( auto const& id_element : element_store )
            {
                auto const& target_id = std::get<0>( id_element );
                auto const& element_impl_variant = std::get<1>( id_element );
                result.emplace_back( elementImplToIntermediateType( target_id, element_impl_variant ) );
            }

            return result;
        }

        template< typename ActionType >
        intermediate_type operator()( ActionType& _action ) const
        {
            auto& element_impl_variant = element_store.at( _action.target_id );
            auto visitor = std::bind( ElementImplActionVisitor{}, std::placeholders::_1, std::ref( _action ) );
            return boost::apply_visitor( visitor, element_impl_variant );
        }
    };

}

#endif //__SDVIZ_ACTION_DISPATCH_HPP__
