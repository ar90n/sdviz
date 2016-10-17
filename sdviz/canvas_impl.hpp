#ifndef __SDVIZ_CANVAS_IMPL_HPP__
# define __SDVIZ_CANVAS_IMPL_HPP__

# include <tuple>
# include <vector>
# include <string>

# include <boost/variant.hpp>

# include "image_impl.hpp"

namespace sdviz
{
    class CanvasImpl final
    {
        public:
            struct LinePolicy
            {
                using param_type = std::tuple< std::vector< int >, uint8_t, uint8_t, uint8_t, uint8_t, bool, bool >;
                static const std::string func_name;
            };

            struct CirclePolicy
            {
                using param_type = std::tuple< int, int, double, uint8_t, uint8_t, uint8_t, uint8_t, bool, bool >;
                static const std::string func_name;
            };

            struct ImagePolicy
            {
                using param_type = std::tuple< ImageImpl, int, int, double >;
                static const std::string func_name;
            };

            struct RectPolicy
            {
                using param_type = std::tuple< int, int, int, int, uint8_t, uint8_t, uint8_t, uint8_t, bool, bool >;
                static const std::string func_name;
            };

            struct TextPolicy
            {
                using param_type = std::tuple< std::string, int, int, uint8_t, uint8_t, uint8_t, uint8_t >;
                static const std::string func_name;
            };

            template< typename CommandPolicy >
            class CanvasCommand final : public CommandPolicy
            {
                public:
                    using CommandPolicy::func_name;
                    using param_type = typename CommandPolicy::param_type;

                    template< typename... ArgTypes >
                    CanvasCommand( ArgTypes const&... _args )
                        : param( _args... )
                    {
                    }

                    param_type getParam() const noexcept
                    {
                        return param;
                    }

                private:
                    param_type param;
            };

            using LineCommand = CanvasCommand< LinePolicy >;
            using CircleCommand = CanvasCommand< CirclePolicy >;
            using ImageCommand = CanvasCommand< ImagePolicy >;
            using RectCommand = CanvasCommand< RectPolicy >;
            using TextCommand = CanvasCommand< TextPolicy >;
            using CanvasCommandVariant = boost::variant< LineCommand,
                                                         CircleCommand,
                                                         ImageCommand,
                                                         RectCommand,
                                                         TextCommand >;

            template< typename CommandType >
            void addCommand( CommandType const& _command )
            {
                commands.emplace_back( _command );
            }

            template< typename CommandType >
            void addCommand( CommandType&& _command )
            {
                commands.emplace_back( std::move( _command ) );
            }

            CanvasImpl( int const _width, int const _height );
            CanvasImpl( CanvasImpl const& _impl ) = default;
            CanvasImpl( CanvasImpl&& _impl ) = default;
            ~CanvasImpl() = default;

            int getWidth() const noexcept;
            int getHeight() const noexcept;
            std::vector< CanvasCommandVariant >::const_iterator cbegin() const noexcept;
            std::vector< CanvasCommandVariant >::const_iterator cend() const noexcept;

            CanvasImpl& operator =( CanvasImpl const& _impl ) = default;
            CanvasImpl& operator =( CanvasImpl&& _impl ) = default;

        private:
            int width;
            int height;
            std::vector< CanvasCommandVariant > commands;
    };
}

#endif // __SDVIZ_CANVAS_IMPL_HPP__
