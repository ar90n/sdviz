#include <sdviz.hpp>

#include <memory>
#include <cmath>
#include <iostream>
#include <algorithm>

extern unsigned char lena[];
extern unsigned int lena_len;
extern unsigned int lena_width;
extern unsigned int lena_height;

sdviz::Image allocateImage( int width, int height )
{
    const int bytes = 3 * width * height;
    sdviz::Image::Format const format = sdviz::Image::Format::RGB_888;
    auto buffer = std::shared_ptr< uint8_t >( new uint8_t[ bytes ] );
    sdviz::Image image( width, height, format, buffer );

    return image;
}

sdviz::Image loadTestImage()
{
    sdviz::Image image = allocateImage( lena_width, lena_height );
    std::copy( lena, lena + lena_len, image.getBuffer() );

    return image;
}

void shiftColors( sdviz::Image const& src_image, sdviz::Image& dst_image, double red, double blue, double green )
{
    uint8_t const* const src_buffer = src_image.getBuffer();
    uint8_t* const dst_buffer = dst_image.getBuffer();
    auto shift_color = []( uint8_t value, double coeff ) {
        double pow_coeff = 1.0 + 8.0 * std::abs( coeff - 0.5 );
        if( 0.5 < coeff )
        {
            pow_coeff = 1.0 / pow_coeff;
        }

        double const normalized = value / 255.0;
        double const shifted = std::pow( normalized, pow_coeff );
        return static_cast< uint8_t >( 255.0 * shifted + 0.5 );
    };

    int const pixels = src_image.getWidth() * src_image.getHeight();
    for( int i = 0; i < pixels; ++i )
    {
        int const base_offset = 3 * i;
        dst_buffer[ base_offset + 0 ] = shift_color( src_buffer[ base_offset + 0 ], red );
        dst_buffer[ base_offset + 1 ] = shift_color( src_buffer[ base_offset + 1 ], green );
        dst_buffer[ base_offset + 2 ] = shift_color( src_buffer[ base_offset + 2 ], blue );
    }
}

sdviz::ChartElement::value_type calcHistogram( sdviz::Image const& _image )
{
    sdviz::ChartElement::value_type::value_type::second_type red_hist( 256, 0.0 );
    sdviz::ChartElement::value_type::value_type::second_type green_hist( 256, 0.0 );
    sdviz::ChartElement::value_type::value_type::second_type blue_hist( 256, 0.0 );

    uint8_t const* const image_buffer = _image.getBuffer();
    int const pixels = _image.getWidth() * _image.getHeight();
    for( int i = 0; i < pixels; ++i )
    {
        int const base_offset = 3 * i;
        red_hist[ image_buffer[ base_offset + 0 ] ] += 1.0;
        green_hist[ image_buffer[ base_offset + 1 ] ] += 1.0;
        blue_hist[ image_buffer[ base_offset + 2 ] ] += 1.0;
    }

    return sdviz::ChartElement::value_type{
        { "red", red_hist },
        { "green", green_hist },
        { "blue", blue_hist }
    };
}

int main(int argc, char const* argv[])
{
    sdviz::Config config;
    config.http_port = 8082;
    config.ws_port = 8083;
    sdviz::start( config );

    sdviz::Image org_image = loadTestImage();
    sdviz::Image image = allocateImage( org_image.getWidth(), org_image.getHeight() );
    size_t const image_size = 3 * org_image.getWidth() * org_image.getHeight();
    std::copy( org_image.getBuffer(), org_image.getBuffer() + image_size, image.getBuffer() );

    sdviz::Canvas canvas{ image.getWidth(), image.getHeight() };
    canvas.drawImage( image, std::make_tuple( 0.0, 0.0 ) );
    auto image_element = sdviz::CanvasElement::create( canvas );

    sdviz::ChartElement::value_type chart_data{ calcHistogram( image ) };
    sdviz::ChartElement chart_element = sdviz::ChartElement::create( chart_data );

    double red_slider_value = 0.5;
    sdviz::SliderElementParam red_slider_param;
    red_slider_param.label = "RED";
    red_slider_param.on_value_changed = [&red_slider_value]( double old_value, double new_value ){
        red_slider_value = new_value;
    };
    sdviz::SliderElement red_slider_element = sdviz::SliderElement::create( red_slider_value, red_slider_param );

    double blue_slider_value = 0.5;
    sdviz::SliderElementParam blue_slider_param;
    blue_slider_param.label = "BLUE";
    blue_slider_param.on_value_changed = [&blue_slider_value]( double old_value, double new_value ){
        blue_slider_value = new_value;
    };
    sdviz::SliderElement blue_slider_element = sdviz::SliderElement::create( blue_slider_value, blue_slider_param );

    double green_slider_value = 0.5;
    sdviz::SliderElementParam green_slider_param;
    green_slider_param.label = "GREEN";
    green_slider_param.on_value_changed = [&green_slider_value]( double old_value, double new_value ){
        green_slider_value = new_value;
    };
    sdviz::SliderElement green_slider_element = sdviz::SliderElement::create( green_slider_value, green_slider_param );

    auto slider_container = sdviz::ContainerElement::create( "RGB Sliders", false );
    slider_container << red_slider_element << green_slider_element << blue_slider_element;

    sdviz::ButtonElementParam apply_button_param ;
    apply_button_param.label = "apply";
    apply_button_param.on_value_changed = [&]( bool new_value, bool old_value ){
        shiftColors( org_image, image, red_slider_value, blue_slider_value, green_slider_value );

        sdviz::Canvas canvas{ image.getWidth(), image.getHeight() };
        canvas.drawImage( image, std::make_tuple( 0.0, 0.0 ) );
        image_element.setValue( canvas );

        sdviz::ChartElement::value_type const chart_data{ calcHistogram( image ) };
        chart_element.setValue( chart_data );
    };
    sdviz::ButtonElement apply_button_element = sdviz::ButtonElement::create( false, apply_button_param );

    sdviz::eout << image_element << chart_element << sdviz::endl
                << slider_container << sdviz::Page::endl
                << apply_button_element << sdviz::Page::endl;

    sdviz::wait();
    return 0;
}
