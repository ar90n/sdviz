#include <sdviz.hpp>

#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>

#include <cstdio>

extern unsigned char rad_image[];
extern unsigned int rad_image_len;
extern unsigned int rad_image_width;
extern unsigned int rad_image_height;

sdviz::Image allocateImage( int width, int height )
{
    const int bytes = 2 * width * height;
    sdviz::Image::Format const format = sdviz::Image::Format::UINT_16;
    auto buffer = std::shared_ptr< uint8_t >( new uint8_t[ bytes ] );
    sdviz::Image image( width, height, format, buffer );

    return image;
}

sdviz::Image loadTestImage()
{
    sdviz::Image image = allocateImage( rad_image_width, rad_image_height );
    std::copy( rad_image, rad_image + rad_image_len, image.getBuffer() );
    return image;
}

int main(int argc, char const* argv[])
{
    sdviz::Config config;
    config.http_port = 8082;
    config.ws_port = 8083;
    sdviz::start( config );

    sdviz::Image image = loadTestImage();
    sdviz::Canvas canvas{ image.getWidth(), image.getHeight() };
    canvas.drawImage( image, std::make_tuple( 0.0, 0.0 ) );
    auto image_element = sdviz::CanvasElement::create( canvas );

    auto text_element = sdviz::TextElement::create( "This image is from http://barre.nom.fr/medical/samples/" );

    sdviz::eout << image_element << sdviz::endl
                << text_element << sdviz::endl;
    sdviz::wait();
    return 0;
}
