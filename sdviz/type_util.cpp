#include <string>

#include "type_util.hpp"

using namespace sdviz;

Image::Format sdviz::convertToImageFormat( ImageImpl::Format const _format )
{
    switch (_format) {
        case ImageImpl::Format::RGB_888:
            return Image::Format::RGB_888;
        case ImageImpl::Format::UINT_8:
            return Image::Format::UINT_8;
    }

    throw std::runtime_error( "Invalid image format." );
}

ImageImpl::Format sdviz::convertToImageImplFormat( Image::Format const _format )
{
    switch (_format) {
        case Image::Format::RGB_888:
            return ImageImpl::Format::RGB_888;
        case Image::Format::UINT_8:
            return ImageImpl::Format::UINT_8;
    }

    throw std::runtime_error( "Invalid image format." );
}

std::string sdviz::convertToChartImplType( ChartElementParam::Type const _type )
{
    switch (_type) {
        case ChartElementParam::Type::Line:
            return std::string{"line"};
        case ChartElementParam::Type::Bar:
            return std::string{"bar"};
        case ChartElementParam::Type::Scatter:
            return std::string{"scatter"};
    }

    throw std::runtime_error( "Invalid chart type." );
}
