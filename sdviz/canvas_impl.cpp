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

std::string const sdviz::CanvasImpl::LinePolicy::func_name = "line";
std::string const sdviz::CanvasImpl::CirclePolicy::func_name = "circle";
std::string const sdviz::CanvasImpl::ImagePolicy::func_name = "image";
std::string const sdviz::CanvasImpl::RectPolicy::func_name = "rect";
std::string const sdviz::CanvasImpl::TextPolicy::func_name = "text";
