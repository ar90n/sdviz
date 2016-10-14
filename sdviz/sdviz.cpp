#include <memory>
#include <tuple>
#include <string>
#include <map>
#include <algorithm>

#include "action.hpp"
#include "context.hpp"
#include "canvas_impl.hpp"
#include "image_impl.hpp"
#include "model_sync_server.hpp"
#include "resource.hpp"
#include "sdviz.hpp"
#include "type_util.hpp"

using namespace sdviz;

namespace
{
    void addElement( std::string const& _container_id, std::string const& _element_id, int _span )
    {
        ActionVariant action{ AddElementImplAction{ _container_id, std::make_tuple( _span, _element_id ) } };
        auto queue_ptr = Context::getInstance().getQueuePtr();
        queue_ptr->push( std::make_tuple( std::move( action ), true ) );
    }
}

Image::Image( int const _width, int const _height, Format const _format, uint8_t* const _buffer )
    : pimpl{ std::make_shared< ImageImpl >( _width, _height, convertToImageImplFormat( _format ), _buffer ) }
{
}

Image::Image( int const _width, int const _height, Format const _format, std::shared_ptr< uint8_t > const _buffer )
    : pimpl{ std::make_shared< ImageImpl >( _width, _height, convertToImageImplFormat( _format ), _buffer ) }
{
}

int Image::getWidth() const noexcept
{
    return pimpl->getWidth();
}

int Image::getHeight() const noexcept
{
    return pimpl->getHeight();
}

Image::Format Image::getFormat() const noexcept
{
    return convertToImageFormat( pimpl->getFormat() );
}

uint8_t* Image::getBuffer() const noexcept
{
    return pimpl->getBuffer();
}

ImageImpl* Image::getImpl() const noexcept
{
    return pimpl.get();
}

sdviz::Canvas::Canvas( int const _width, int const _height )
    : pimpl( std::make_shared< CanvasImpl >( _width, _height ) )
{
}

void sdviz::Canvas::drawImage( Image const& _image, value_type const& _pos, double _opacity )
{
    double opacity = std::min( std::max( 0.0, _opacity ), 1.0 );
    ImageImpl image_impl{ _image.getImpl()->clone() };
    auto draw_image = CanvasImpl::ImageCommand( std::move( image_impl ), std::get<0>( _pos ), std::get<1>( _pos ), opacity );
    pimpl->addCommand( std::move( draw_image ) );
}

void sdviz::Canvas::drawRect( value_type const& _lt, value_type const& _rb, color_type _color, uint8_t _line_width, bool _fill, bool _with_dots )
{
    auto const draw_rect = CanvasImpl::RectCommand( std::get<0>( _lt ),
                                                    std::get<1>( _lt ),
                                                    std::get<0>( _rb ),
                                                    std::get<1>( _rb ),
                                                    std::get<0>( _color ),
                                                    std::get<1>( _color ),
                                                    std::get<2>( _color ),
                                                    _line_width,
                                                    _fill,
                                                    _with_dots );
    pimpl->addCommand( std::move( draw_rect ) );
}

void sdviz::Canvas::drawCircle( value_type const& _center, double _radius, color_type _color, uint8_t _line_width, bool _fill, bool _with_dots )
{
    auto const draw_circle = CanvasImpl::CircleCommand( std::get<0>( _center ),
                                                        std::get<1>( _center ),
                                                        _radius,
                                                        std::get<0>( _color ),
                                                        std::get<1>( _color ),
                                                        std::get<2>( _color ),
                                                        _line_width,
                                                        _fill,
                                                        _with_dots );
    pimpl->addCommand( std::move( draw_circle ) );
}

void sdviz::Canvas::drawText( std::string const& _text, value_type const& _pos, color_type _color, uint8_t _font_size )
{
    auto const draw_text = CanvasImpl::TextCommand( _text,
                                                    std::get<0>( _pos ),
                                                    std::get<1>( _pos ),
                                                    std::get<0>( _color ),
                                                    std::get<1>( _color ),
                                                    std::get<2>( _color ),
                                                    _font_size );
    pimpl->addCommand( std::move( draw_text ) );
}

void sdviz::Canvas::drawLine( container_type< value_type > const& _points, color_type _color, uint8_t _line_width, bool _fill, bool _with_dots )
{
    std::vector< int > serialized_points;
    serialized_points.reserve( _points.size() * 2 );
    std::for_each( std::begin( _points ), std::end( _points ), [&serialized_points]( value_type const& _point ){
        serialized_points.emplace_back( std::get<0>( _point ) );
        serialized_points.emplace_back( std::get<1>( _point ) );
    });

    auto const draw_line = CanvasImpl::LineCommand( serialized_points,
                                                    std::get<0>( _color ),
                                                    std::get<1>( _color ),
                                                    std::get<2>( _color ),
                                                    _line_width,
                                                    _fill,
                                                    _with_dots );
    pimpl->addCommand( std::move( draw_line ) );
}

int sdviz::Canvas::getWidth() const noexcept
{
    return pimpl->getWidth();
}

int sdviz::Canvas::getHeight() const noexcept
{
    return pimpl->getHeight();
}

CanvasImpl* sdviz::Canvas::getImpl() const noexcept
{
    return pimpl.get();
}

template< typename ValueType, typename ParamType >
Element< ValueType, ParamType >::Element( std::string const& _id )
    : id( _id )
{
}

template< typename ValueType, typename ParamType >
Element< ValueType, ParamType > Element< ValueType, ParamType >::create( value_type const& _value, param_type const& _param )
{
    using impl_type = typename ImplTypeTraits< Element >::type;
    using set_element_action_type = Action< ElementImplVariant >;

    auto id = generateElmenetId();
    auto element = Element( id );

    typename impl_type::value_type impl_value{ convertToImplValue< Element >( _value ) };
    typename impl_type::param_type impl_param{ convertToImplParam< Element >( _param ) };
    impl_type element_impl{ std::move( impl_value ), std::move( impl_param ) };
    ActionVariant action{ set_element_action_type{ id, ElementImplVariant{ std::move( element_impl ) } } };

    auto queue_ptr = Context::getInstance().getQueuePtr();
    queue_ptr->push( std::make_tuple( std::move( action ), true ) );
    return element;
}

template< typename ValueType, typename ParamType >
void Element< ValueType, ParamType >::setValue( value_type const& _value )
{
    using impl_type = typename ImplTypeTraits< Element >::type;
    using set_value_action_type = typename ActionTypeTraits< impl_type >::set_value_type;

    typename impl_type::value_type impl_value{ convertToImplValue< Element >( _value ) };
    ActionVariant action{ set_value_action_type{ id, std::move(impl_value) } };

    auto queue_ptr = Context::getInstance().getQueuePtr();
    queue_ptr->push( std::make_tuple( std::move( action ), true ) );
}

template< typename ValueType, typename ParamType >
void Element< ValueType, ParamType >::setParam( param_type const& _param )
{
    using impl_type = typename ImplTypeTraits< Element >::type;
    using set_param_action_type = typename ActionTypeTraits< impl_type >::set_param_type;

    typename impl_type::param_type impl_param{ convertToImplParam< Element >( _param ) };
    ActionVariant action{ set_param_action_type{ id, std::move(impl_param) } };

    auto queue_ptr = Context::getInstance().getQueuePtr();
    queue_ptr->push( std::make_tuple( std::move( action ), true ) );
}

ContainerElement::ContainerElement( std::string const& _id, std::string const& _label, bool _is_row_direction )
    : id{ _id },
      label{ _label },
      _is_row_direction{ _is_row_direction }
{
}

ContainerElement ContainerElement::create( std::string const& _label, bool _is_row_direction )
{
    using set_element_action_type = Action< ElementImplVariant >;

    auto id = generateElmenetId();
    auto element = ContainerElement( id, _label, _is_row_direction );

    ContainerElementImpl element_impl{ LayoutImpl{}, ContainerElementImplParam{ _label, _is_row_direction } };
    ActionVariant action{ set_element_action_type{ id, ElementImplVariant{ std::move( element_impl ) } } };

    auto queue_ptr = Context::getInstance().getQueuePtr();
    queue_ptr->push( std::make_tuple( std::move( action ), true ) );
    return element;
}

void ContainerElement::addElement( std::string const& _id )
{
    ::addElement( id, _id, 1 );
}

Page::Page()
    : span{1}
{
}

Page& sdviz::Page::getInstance()
{
    static Page instance;
    return instance;
}

void Page::endl( Page& _page )
{
    _page.span = -1;
    _page.addElement( dummy_id );
}

std::function< void( Page& ) > Page::setw( int _w )
{
    auto set_width_func = [_w]( Page& _page ){
        _page.span = _w;
    };
    return set_width_func;
}

void Page::addElement( std::string const& _id )
{
    ::addElement( page_id, _id, span );
    span = 1;
}

bool sdviz::start( sdviz::Config const& _config )
{
    Context::getInstance().start();
    ModelSyncServer::getInstance().start( _config.http_port,
                                          _config.http_threads,
                                          _config.ws_port,
                                          _config.ws_threads );
    return true;
}

void sdviz::wait()
{
    ModelSyncServer::getInstance().wait();
    Context::getInstance().wait();
}

void sdviz::stop()
{
    ModelSyncServer::getInstance().stop();
    Context::getInstance().stop();
}

template class sdviz::Element< std::string, sdviz::TextElementParam >;
template class sdviz::Element< Canvas, sdviz::CanvasElementParam >;
template class sdviz::Element< std::map< std::string, std::vector< double > >, sdviz::ChartElementParam >;
template class sdviz::Element< bool, sdviz::ButtonElementParam >;
template class sdviz::Element< double, sdviz::SliderElementParam >;
