#ifndef __SDVIZ_IMAGE_IMPL_HPP__
# define __SDVIZ_IMAGE_IMPL_HPP__

#include <memory>

namespace sdviz
{
    class ImageImpl final
    {
        public:
            enum Format
            {
                RGB_888,
                UINT_8,
                UINT_16
            };

            static int GetChannelsPerPixel( ImageImpl const& _image_impl );
            static size_t GetBytesPerChannel( ImageImpl const& _image_impl );
            static size_t GetBufferSize( ImageImpl const& _image_impl );

            ImageImpl( int const _width, int const _height, Format const _format, uint8_t* const _buffer = nullptr );
            ImageImpl( int const _width, int const _height, Format const _format, std::shared_ptr< uint8_t > const _buffer );
            ImageImpl( ImageImpl const& _image_impl ) = default;
            ImageImpl( ImageImpl&& _image_impl ) = default;
            ~ImageImpl() = default;

            ImageImpl clone() const;
            int getWidth() const noexcept;
            int getHeight() const noexcept;
            Format getFormat() const noexcept;
            uint8_t* getBuffer() const noexcept;

            ImageImpl& operator =( ImageImpl const& _image_impl ) = default;
            ImageImpl& operator =( ImageImpl&& _image_impl ) = default;

        private:
            int width;
            int height;
            Format format;
            std::shared_ptr< uint8_t > buffer;
    };
}

#endif // __SDVIZ_IMAGE_IMPL_HPP__
