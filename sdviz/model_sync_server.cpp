#include <boost/lexical_cast.hpp>

#include "action.hpp"
#include "context.hpp"
#include "log.hpp"
#include "model_sync_server.hpp"
#include "resource.hpp"
#include "serdes.hpp"
#include "log.hpp"

using namespace sdviz;

ModelSyncServer& ModelSyncServer::getInstance()
{
    static ModelSyncServer server;
    return server;
}

void ModelSyncServer::start( int const _http_port,
                             int const _http_threads,
                             int const _ws_port,
                             int const _ws_threads )
{
    http_server_ptr = std::make_unique< HttpServer >( _http_port, _http_threads );
    http_server_ptr->resource["^/config$"]["GET"]=[&,_ws_port]( std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> ) {
        std::stringstream ss;
        ss << "{" << R"("ws_port")" << ":" << _ws_port << "}";
        std::string const content{ss.str()};

        *response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.size() << "\r\n\r\n" << content;
        response->flush();
    };
    http_server_ptr->default_resource["GET"]=[&]( std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> ) {
        std::string const& content = getMainPage();
        *response << "HTTP/1.1 200 OK\r\nContent-Length: " << content.size() << "\r\n\r\n" << content;
        response->flush();
    };

    ws_server_ptr = std::make_unique< WsServer >( _ws_port, _ws_threads );
    auto& ws_endpoint = ws_server_ptr->endpoint["^/$"];
    ws_endpoint.onopen=[&]( std::shared_ptr<WsServer::Connection> connection) {
        std::string const con_hash = hashConnection( connection );
        LOG(info) << "Server: Opened connection " << con_hash << ".";

        ActionVariant action{  SyncAction{ dummy_id, nullptr } };
        auto queue_ptr = Context::getInstance().getQueuePtr();
        queue_ptr->push( std::make_tuple( std::move( action ), true ) );
    };

    ws_endpoint.onmessage=[&]( std::shared_ptr<WsServer::Connection>, std::shared_ptr<WsServer::Message> message) {
        try
        {
            std::string message_str{ message->string() };
            auto intermediate_action = deserialize( serialized_type( message_str.begin(), message_str.end() ) );
            receiveAction( intermediate_action );
        }
        catch( std::exception& e )
        {
            LOG(error) << e.what();
        }
        catch( ... )
        {
            LOG(error) << "Exception catched.";
        }
    };

    //See RFC 6455 7.4.1. for status codes
    ws_endpoint.onclose=[&]( std::shared_ptr<WsServer::Connection> connection, int, std::string const& ) {
        std::string const con_hash = hashConnection( connection );
        LOG(info) << "Server: Closed connection " << con_hash << ".";
    };

    //See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
    ws_endpoint.onerror=[&]( std::shared_ptr<WsServer::Connection> connection, const boost::system::error_code& ec) {
        std::string const con_hash = hashConnection( connection );
        LOG(info) << "Server: Error in connection " << con_hash << ". " << "Error: " << ec << ", error message: " << ec.message();
    };

    http_server_thread = std::thread([&](){
            http_server_ptr->start();
    });

    ws_server_thread = std::thread([&](){
            ws_server_ptr->start();
    });
}

void ModelSyncServer::wait()
{
    if( http_server_thread.joinable() )
    {
        http_server_thread.join();
    }

    if( ws_server_thread.joinable() )
    {
        ws_server_thread.join();
    }
}

void ModelSyncServer::stop()
{
    if( http_server_ptr )
    {
        http_server_ptr->stop();
    }

    if( ws_server_ptr )
    {
        ws_server_ptr->stop();
    }

    wait();
}

void ModelSyncServer::sendAction( intermediate_type const& _intermediate_action )
{
    if( !isValid( _intermediate_action ) )
    {
        return;
    }
    serialized_type buffer{ serialize( _intermediate_action ) };

    auto send_stream = std::make_shared<WsServer::SendStream>();
    std::copy( std::begin(buffer), std::end(buffer), std::ostream_iterator<serialized_type::value_type>(*send_stream));

    auto& ws_endpoint = ws_server_ptr->endpoint["^/$"];
    auto connections = ws_endpoint.get_connections();
    for( auto& con : connections )
    {
        ws_server_ptr->send( con, send_stream, [](const boost::system::error_code& ec){
            if(ec) {
                LOG(error) << "Server: Error sending message. Error: " << ec << ", error message: " << ec.message();
            }
        }, 130);
    }
}

std::string ModelSyncServer::hashConnection( std::shared_ptr< WsServer::Connection > const& _connection ) const
{
    return boost::lexical_cast< std::string >( reinterpret_cast< size_t >( _connection.get() ) );
}

void ModelSyncServer::receiveAction( intermediate_type const& _intermediate_action )
{
    if( !isValid( _intermediate_action ) )
    {
        return;
    }

    ActionVariant action = intermediateTypeToSetValueAction( _intermediate_action );
    auto queue_ptr = Context::getInstance().getQueuePtr();
    queue_ptr->push( std::make_tuple( std::move( action ), true ) );
}
