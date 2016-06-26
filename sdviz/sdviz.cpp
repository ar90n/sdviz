#include <memory>
#include <tuple>
#include <string>
#include <map>

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
    void addStyleCommands( std::shared_ptr< CanvasImpl > _impl, Canvas::style const& _style )
    {
        std::stringstream ss;
        ss << "rgb(" << static_cast<int>(_style.color[0] ) << ","
                     << static_cast<int>(_style.color[1] ) << ","
                     << static_cast<int>(_style.color[2] ) << ")";
        auto const color_str = ss.str();ss.str("");
        auto const fill_style = CanvasImpl::SetPropertyCommand( "fillStyle", color_str );
        _impl->addCommand( std::move( fill_style ) );

        auto const stroke_style = CanvasImpl::SetPropertyCommand( "strokeStyle", color_str );
        _impl->addCommand( std::move( stroke_style ) );

        auto const line_width_str = std::to_string( _style.line_width );
        auto const line_width = CanvasImpl::SetPropertyCommand( "lineWidth", line_width_str );
        _impl->addCommand( std::move( line_width ) );

        ss << _style.font_size << "px 'sans serif'";
        auto const font_str = ss.str(); ss.str("");
        auto const set_font = CanvasImpl::SetPropertyCommand( "font", font_str );
        _impl->addCommand( std::move( set_font ) );
    }

    void addRenderCommands(std::shared_ptr< CanvasImpl > _impl, Canvas::style const& _style )
    {
        if( _style.fill )
        {
            _impl->addCommand( CanvasImpl::FillCommand() );
        }
        else
        {
            _impl->addCommand( CanvasImpl::StrokeCommand() );
        }
    }

    template< typename T >
    void drawDots( std::shared_ptr< CanvasImpl > _impl, T const& _points, sdviz::Canvas::style const& _style  )
    {
        auto begin_path = CanvasImpl::BeginPathCommand();
        _impl->addCommand( std::move( begin_path ) );

        auto beg = std::rbegin( _points );
        auto end = std::rend( _points );
        ++beg;
        while( beg != end )
        {
            auto const move_to_command = CanvasImpl::MoveToCommand( std::get<0>( *beg ), std::get<1>( *beg ) );
            _impl->addCommand( std::move( move_to_command ) );

            auto const dot_radius = _style.line_width;
            auto const draw_dot = CanvasImpl::ArcCommand( std::get<0>( *beg ),
                                                          std::get<1>( *beg ),
                                                          dot_radius,
                                                          0,
                                                          2 * 3.141596,
                                                          false );
            _impl->addCommand( std::move( draw_dot ) );
            ++beg;
        }

        _impl->addCommand( CanvasImpl::FillCommand() );
    }

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

void sdviz::Canvas::drawImage( Image const& _image, value_type const& _pos, double _opacity, style const& _style )
{
    addStyleCommands( pimpl, _style );

    double opacity = std::min( std::max( 0.0, _opacity ), 1.0 );
    ImageImpl image_impl{ _image.getImpl()->clone() };
    auto draw_image = CanvasImpl::PutImageDataCommand( std::move( image_impl ), std::get<0>( _pos ), std::get<1>( _pos ), opacity );
    pimpl->addCommand( std::move( draw_image ) );
}

void sdviz::Canvas::drawRect( value_type const& _lt, value_type const& _rb, style const& _style )
{
    addStyleCommands( pimpl, _style );

    auto const begin_path = CanvasImpl::BeginPathCommand();
    pimpl->addCommand( std::move( begin_path ) );

    auto const move = CanvasImpl::MoveToCommand( std::get<0>( _lt ), std::get<1>( _lt ) );
    pimpl->addCommand( std::move( move ) );

    auto const draw_rect = CanvasImpl::RectCommand( std::get<0>( _lt ),
                                                    std::get<1>( _lt ),
                                                    std::get<0>( _rb ),
                                                    std::get<1>( _rb ) );
    pimpl->addCommand( std::move( draw_rect ) );

    addRenderCommands( pimpl, _style );
}

void sdviz::Canvas::drawCircle( value_type const& _center, double _radius, style const& _style )
{
    addStyleCommands( pimpl, _style );

    auto const begin_path = CanvasImpl::BeginPathCommand();
    pimpl->addCommand( std::move( begin_path ) );

    auto const move = CanvasImpl::MoveToCommand( std::get<0>( _center ) + _radius, std::get<1>( _center ) );
    pimpl->addCommand( std::move( move ) );

    auto const draw_circle = CanvasImpl::ArcCommand( std::get<0>( _center ),
                                                     std::get<1>( _center ),
                                                     _radius,
                                                     0,
                                                     2 * 3.1416,
                                                     false );
    pimpl->addCommand( std::move( draw_circle ) );

    addRenderCommands( pimpl, _style );
}

void sdviz::Canvas::drawText( std::string const& _text, value_type const& _pos, style const& _style )
{
    addStyleCommands( pimpl, _style );

    if( _style.fill )
    {
        auto draw_text = CanvasImpl::FillTextCommand( _text, std::get<0>( _pos ), std::get<1>( _pos ) );
        pimpl->addCommand( std::move( draw_text ) );
    }
    else
    {
        auto draw_text = CanvasImpl::StrokeTextCommand( _text, std::get<0>( _pos ), std::get<1>( _pos ) );
        pimpl->addCommand( std::move( draw_text ) );
    }
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

void sdviz::Canvas::drawLine( container_type< value_type > const& _points, style const& _style )
{
    auto beg = std::begin( _points );
    auto end = std::end( _points );
    if( std::distance( beg, end ) < 2 )
    {
        return;
    }

    addStyleCommands( pimpl, _style );

    auto const begin_path = CanvasImpl::BeginPathCommand();
    pimpl->addCommand( std::move( begin_path ) );

    auto const move_to_command = CanvasImpl::MoveToCommand( std::get<0>( *beg ), std::get<1>( *beg ) );
    pimpl->addCommand( std::move( move_to_command ) );
    beg++;

    std::for_each( beg, end, [&]( value_type const& _point ){
        auto const line_to_command = CanvasImpl::LineToCommand( std::get<0>( _point ), std::get<1>( _point ) );
        pimpl->addCommand( std::move( line_to_command ) );
    });

    addRenderCommands( pimpl, _style );

    if( _style.with_dots )
    {
        drawDots( pimpl, _points, _style );
    }
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
