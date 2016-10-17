#ifndef _SDVIZ_HPP_
# define _SDVIZ_HPP_

# include <string>
# include <map>
# include <memory>
# include <vector>
# include <cstdint>
# include <stdexcept>
# include <initializer_list>

namespace sdviz
{
    struct Config
    {
        int http_port = 8080;
        int http_threads = 2;
        int ws_port = 8888;
        int ws_threads = 2;
    };

    class ImageImpl;
    class Image final
    {
        public:
            enum Format
            {
                RGB_888,
                UINT_8,
                UINT_16
            };

            Image( int const _width, int const _height, Format const _format, uint8_t* const _buffer = nullptr );
            Image( int const _width, int const _height, Format const _format, std::shared_ptr<uint8_t> const _buffer );
            Image( Image const& _image_impl ) = default;
            Image( Image&& _image_impl ) = default;
            ~Image() = default;

            int getWidth() const noexcept;
            int getHeight() const noexcept;
            Format getFormat() const noexcept;
            uint8_t* getBuffer() const noexcept;
            ImageImpl* getImpl() const noexcept;

            Image& operator =( Image const& _image_impl ) = default;
            Image& operator =( Image&& _image_impl ) = default;

        private:
            std::shared_ptr< ImageImpl > pimpl;
    };

    class CanvasImpl;
    class Canvas final
    {
        public:
            using value_type = std::tuple< int, int >;
            using color_type = std::tuple< uint8_t, uint8_t, uint8_t >;
            template< typename T > using container_type = std::vector< T >;

            struct style
            {
                style() {}

                uint8_t color[3] = { 0x00, 0x00, 0x00 };
                int line_width = 1;
                int font_size = 24;
                bool fill = false;
                bool with_dots = false;
            };

            Canvas( int const _width, int const _height );
            Canvas( Canvas const& _canvas ) = default;
            Canvas( Canvas&& _canvas ) = default;
            ~Canvas() = default;

            void drawImage( Image const& _image,
                            value_type const& _lt,
                            double _opacity = 1.0 );
            void drawRect( value_type const& _lt,
                           value_type const& _rb,
                           color_type color = color_type{ 0x00, 0x00, 0x00 },
                           uint8_t line_width = 1,
                           bool fill = false,
                           bool with_dots = false );
            void drawCircle( value_type const& _center,
                             double _radius,
                             color_type color = color_type{ 0x00, 0x00, 0x00 },
                             uint8_t line_width = 1,
                             bool fill = false,
                             bool with_dots = false );
            void drawText( std::string const& _text,
                           value_type const& _pos,
                           color_type color = color_type{ 0x00, 0x00, 0x00 },
                           uint8_t font_size = 24 );
            void drawLine( container_type< value_type > const& _points,
                           color_type color = color_type{ 0x00, 0x00, 0x00 },
                           uint8_t line_width = 1,
                           bool fill = false,
                           bool with_dots = false );
            template< typename IteratorType >
            void drawLine( IteratorType _begin,
                           IteratorType _end,
                           color_type color = color_type{ 0x00, 0x00, 0x00 },
                           uint8_t line_width = 1,
                           bool fill = false,
                           bool with_dots = false );

            int getWidth() const noexcept;
            int getHeight() const noexcept;
            CanvasImpl* getImpl() const noexcept;

            Canvas& operator =( Canvas const& _canvas ) = default;
            Canvas& operator =( Canvas&& _canvas ) = default;

        private:
            std::shared_ptr< CanvasImpl > pimpl;
    };

    template< typename IteratorType >
    void Canvas::drawLine( IteratorType _begin,
                           IteratorType _end,
                           color_type _color,
                           uint8_t _line_width,
                           bool _fill,
                           bool _with_dots )
    {
        container_type< value_type > points( _begin, _end );
        drawLine( points, _color, _line_width, _fill, _with_dots );
    }

    template< typename ValueType, typename ParamType >
    class Element final
    {
        public:
            using value_type = ValueType;
            using param_type = ParamType;

            static Element create( value_type const& _value, param_type const& _param = param_type{} );
            void setValue( value_type const& _value );
            void setParam( param_type const& _param );

            Element( Element const& _element ) = default;
            Element( Element&& _element ) = default;
            ~Element() = default;

            Element& operator =( Element const& _element ) = default;
            Element& operator =( Element&& _element ) = default;

            std::string const id;
        private:
            Element( std::string const& _id );
    };

    struct TextElementParam final
    {
    };
    using TextElement = Element< std::string, TextElementParam >;

    struct CanvasElementParam final
    {
    };
    using CanvasElement = Element< Canvas, CanvasElementParam >;

    struct ChartElementParam final
    {
        enum Type
        {
            Line,
            Bar,
            Scatter
        };

        Type type;
        std::map< std::string, std::string > value_map;
    };
    using ChartElement = Element< std::map< std::string, std::vector< double > >, ChartElementParam >;

    struct ButtonElementParam final
    {
        std::string label;
        std::function< void( bool, bool ) > on_value_changed = []( bool, bool ){};
    };
    using ButtonElement = Element< bool, ButtonElementParam >;

    struct SliderElementParam final
    {
        std::string label;
        std::function< void( double, double ) > on_value_changed = []( double, double ){};
    };
    using SliderElement = Element< double, SliderElementParam >;

    class ContainerElement
    {
        public:
            static ContainerElement create( std::string const& _label = std::string{}, bool _is_row_direction = true );

            ContainerElement( ContainerElement const& ) = default;
            ContainerElement( ContainerElement&& ) = default;
            ~ContainerElement() = default;

            ContainerElement& operator=( ContainerElement const& ) = default;
            ContainerElement& operator=( ContainerElement&& ) = default;

            template< typename ValueType, typename ParamType >
            ContainerElement& operator <<( Element< ValueType, ParamType > const& _element )
            {
                addElement( _element.id );
                return *this;
            }

            std::string const id;
            std::string const label;
            bool const _is_row_direction;

        protected:
            ContainerElement( std::string const& _id, std::string const& _label, bool _is_row_direction );
            void addElement( std::string const& _id );
    };

    class Page
    {
        public:
            static Page& getInstance();
            static void endl( Page& _page );
            static std::function< void( Page& ) > setw( int _w );

            template< typename ValueType, typename ParamType >
            Page& operator <<( Element< ValueType, ParamType > const& _element )
            {
                auto container = sdviz::ContainerElement::create();
                container << _element;
                addElement( container.id );
                return *this;
            }

            Page& operator <<( std::string const& _text )
            {
                *this << sdviz::TextElement::create( _text );
                return *this;
            }

            Page& operator <<( Image const& _image )
            {
                sdviz::Canvas canvas{ _image.getWidth(), _image.getHeight() };
                canvas.drawImage( _image, std::make_tuple( 0.0, 0.0 ) );
                *this << sdviz::CanvasElement::create( canvas );
                return *this;
            }

            Page& operator <<( ContainerElement const& _container )
            {
                addElement( _container.id );
                return *this;
            }

            Page& operator <<( std::function< void( Page& ) > _func )
            {
                _func( *this );
                return *this;
            }

        private:
            Page();
            void addElement( std::string const& _id );
            int span;
    };
    static auto& endl =  Page::endl;
    static auto& setw =  Page::setw;
    static Page& eout = Page::getInstance();

    bool start( Config const& _config = Config{} );
    void wait();
    void stop();
}

#endif // _SDVIZ_HPP_
