#include "image_impl.hpp"

#include <cstdlib>
#include <stdexcept>
#include <algorithm>

using namespace sdviz;

namespace
{
    int GetChannelsPerPixel( ImageImpl::Format _format )
    {
        switch( _format )
        {
            case ImageImpl::Format::RGB_888:
                return 3;
            case ImageImpl::Format::UINT_8:
            case ImageImpl::Format::UINT_16: // fall through
                return 1;
        }

        throw std::runtime_error( "Invalid image format." );
    }

    size_t GetBytesPerChannel( ImageImpl::Format _format )
    {
        switch( _format )
        {
            case ImageImpl::Format::RGB_888:
            case ImageImpl::Format::UINT_8: // fall through
                return 1;
            case ImageImpl::Format::UINT_16:
                return 2;
        }

        throw std::runtime_error( "Invalid image format." );
    }

    size_t GetBufferSize( int const _width, int const _height, ImageImpl::Format const _format )
    {
        return _width * _height * GetChannelsPerPixel( _format ) * GetBytesPerChannel( _format );
    }
}

sdviz::ImageImpl:: ImageImpl( int const _width, int const _height, Format const _format, uint8_t* const _buffer )
    : width( _width ),
      height( _height ),
      format( _format ),
      buffer( ( _buffer == nullptr ) ? std::shared_ptr< uint8_t >( new uint8_t[::GetBufferSize( _width, _height, _format )] )
                                     : std::shared_ptr< uint8_t >( _buffer, [](auto*){} ) )
{
}

sdviz::ImageImpl::ImageImpl( int const _width, int const _height, Format const _format, std::shared_ptr< uint8_t > const _buffer )
    : width( _width ),
      height( _height ),
      format( _format ),
      buffer( ( _buffer.get() == nullptr ) ? std::shared_ptr< uint8_t >( new uint8_t[::GetBufferSize( _width, _height, _format )] )
                                           : std::shared_ptr< uint8_t >( _buffer ) )
{
}

sdviz::ImageImpl sdviz::ImageImpl::clone() const
{
    ImageImpl new_image_impl( width, height, format );
    size_t buffer_size = GetBufferSize( new_image_impl );
    std::copy( buffer.get(), buffer.get() + buffer_size, new_image_impl.buffer.get() );
    return new_image_impl;
}

int sdviz::ImageImpl::getWidth() const noexcept
{
    return width;
}

int sdviz::ImageImpl::getHeight() const noexcept
{
    return height;
}

sdviz::ImageImpl::Format sdviz::ImageImpl::getFormat() const noexcept
{
    return format;
}

uint8_t* sdviz::ImageImpl::getBuffer() const noexcept
{
    return buffer.get();
}

int sdviz::ImageImpl::GetChannelsPerPixel( ImageImpl const& _image_impl )
{
    return ::GetChannelsPerPixel( _image_impl.format );
}

size_t sdviz::ImageImpl::GetBytesPerChannel( ImageImpl const& _image_impl )
{
    return ::GetBytesPerChannel( _image_impl.format );
}

size_t sdviz::ImageImpl::GetBufferSize( ImageImpl const& _image_impl )
{
    return ::GetBufferSize( _image_impl.width, _image_impl.height, _image_impl.format );
}
