#ifndef __SDVIZ_MODEL_SYNC_SERVER__
# define __SDVIZ_MODEL_SYNC_SERVER__

# include <memory>
# include <thread>
# include <string>

# include <server_http.hpp>
# include <server_ws.hpp>

# include "serdes.hpp"

namespace sdviz
{
    class ModelSyncServer final
    {
        using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
        using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

        public:
            ModelSyncServer( ModelSyncServer const& _server ) = delete;
            ModelSyncServer( ModelSyncServer&& _server ) = delete;
            ~ModelSyncServer() = default;

            ModelSyncServer& operator =( ModelSyncServer const& _server ) = delete;
            ModelSyncServer& operator =( ModelSyncServer&& _server ) = delete;

            static ModelSyncServer& getInstance();

            void start( int const _http_port,
                        int const _http_threads,
                        int const _ws_port,
                        int const _ws_threads );

            void wait();
            void stop();
            void sendAction( intermediate_type const& _intermediate_action );

        private:
            ModelSyncServer() = default;

            std::unique_ptr< HttpServer > http_server_ptr;
            std::unique_ptr< WsServer > ws_server_ptr;
            std::thread http_server_thread;
            std::thread ws_server_thread;

            std::string hashConnection( std::shared_ptr< WsServer::Connection > const& _connection ) const;
            void receiveAction( intermediate_type const& _intermediate_action );
    };
}

#endif // __SDVIZ_MODEL_SYNC_SERVER__
