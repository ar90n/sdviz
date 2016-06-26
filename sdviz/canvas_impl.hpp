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
            struct BeginPathPolicy
            {
                using param_type = std::tuple<>;
                static const std::string func_name;
            };

            struct ClosePathPolicy
            {
                using param_type = std::tuple<>;
                static const std::string func_name;
            };

            struct StrokePolicy
            {
                using param_type = std::tuple<>;
                static const std::string func_name;
            };

            struct FillPolicy
            {
                using param_type = std::tuple<>;
                static const std::string func_name;
            };

            struct MoveToPolicy
            {
                using param_type = std::tuple< int, int >;
                static const std::string func_name;
            };

            struct LineToPolicy
            {
                using param_type = std::tuple< int, int >;
                static const std::string func_name;
            };

            struct ArcPolicy
            {
                using param_type = std::tuple< int, int, double, double, double, bool >;
                static const std::string func_name;
            };

            struct PutImageDataPolicy
            {
                using param_type = std::tuple< ImageImpl, int, int, double >;
                static const std::string func_name;
            };

            struct RectPolicy
            {
                using param_type = std::tuple< int, int, int, int >;
                static const std::string func_name;
            };

            struct StrokeTextPolicy
            {
                using param_type = std::tuple< std::string, int, int >;
                static const std::string func_name;
            };

            struct FillTextPolicy
            {
                using param_type = std::tuple< std::string, int, int >;
                static const std::string func_name;
            };

            struct SetPropertyPolicy
            {
                using param_type = std::tuple< std::string, std::string >;
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

            using BeginPathCommand = CanvasCommand< BeginPathPolicy >;
            using ClosePathCommand = CanvasCommand< ClosePathPolicy >;
            using StrokeCommand = CanvasCommand< StrokePolicy >;
            using FillCommand = CanvasCommand< FillPolicy >;
            using MoveToCommand = CanvasCommand< MoveToPolicy >;
            using LineToCommand = CanvasCommand< LineToPolicy >;
            using ArcCommand = CanvasCommand< ArcPolicy >;
            using PutImageDataCommand = CanvasCommand< PutImageDataPolicy >;
            using RectCommand = CanvasCommand< RectPolicy >;
            using StrokeTextCommand = CanvasCommand< StrokeTextPolicy >;
            using FillTextCommand = CanvasCommand< FillTextPolicy >;
            using SetPropertyCommand = CanvasCommand< SetPropertyPolicy >;
            using CanvasCommandVariant = boost::variant< BeginPathCommand,
                                                         ClosePathCommand,
                                                         StrokeCommand,
                                                         FillCommand,
                                                         MoveToCommand,
                                                         LineToCommand,
                                                         ArcCommand,
                                                         PutImageDataCommand,
                                                         RectCommand,
                                                         StrokeTextCommand,
                                                         FillTextCommand,
                                                         SetPropertyCommand >;

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
