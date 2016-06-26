#include "canvas_impl.hpp"

sdviz::CanvasImpl::CanvasImpl( int const _width, int const _height )
    : width( _width ),
      height( _height )
{
}

int sdviz::CanvasImpl::getWidth() const noexcept
{
    return width;
}

int sdviz::CanvasImpl::getHeight() const noexcept
{
    return  height;
}

std::vector< sdviz::CanvasImpl::CanvasCommandVariant >::const_iterator sdviz::CanvasImpl::cbegin() const noexcept
{
    return commands.cbegin();
}

std::vector< sdviz::CanvasImpl::CanvasCommandVariant >::const_iterator sdviz::CanvasImpl::cend() const noexcept
{
    return commands.cend();
}

std::string const sdviz::CanvasImpl::BeginPathPolicy::func_name = "beginPath";
std::string const sdviz::CanvasImpl::ClosePathPolicy::func_name = "closePath";
std::string const sdviz::CanvasImpl::StrokePolicy::func_name = "stroke";
std::string const sdviz::CanvasImpl::FillPolicy::func_name = "fill";
std::string const sdviz::CanvasImpl::MoveToPolicy::func_name = "moveTo";
std::string const sdviz::CanvasImpl::LineToPolicy::func_name = "lineTo";
std::string const sdviz::CanvasImpl::ArcPolicy::func_name = "arc";
std::string const sdviz::CanvasImpl::PutImageDataPolicy::func_name = "putImageData";
std::string const sdviz::CanvasImpl::RectPolicy::func_name = "rect";
std::string const sdviz::CanvasImpl::StrokeTextPolicy::func_name = "strokeText";
std::string const sdviz::CanvasImpl::FillTextPolicy::func_name = "fillText";
std::string const sdviz::CanvasImpl::SetPropertyPolicy::func_name = "setProperty";
