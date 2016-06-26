#include <tuple>

#include "./action.hpp"
#include "./context.hpp"
#include "./log.hpp"
#include "./model_sync_server.hpp"
#include "./resource.hpp"
#include "./serdes.hpp"
#include "./visitor.hpp"
#include "./variant_util.hpp"

using namespace sdviz;

Context& Context::getInstance()
{
    static Context context;
    return context;
}

void Context::start()
{
    stop();
    loop_thread = std::thread([&](){
        is_loop.store( true, std::memory_order_release );
        while( is_loop.load( std::memory_order_acquire ) )
        {
            auto& action = std::get<0>( action_queue_ptr->front() );
            bool is_sync_with_client = std::get<1>( action_queue_ptr->front() );

            try
            {
                auto intermediate_result = boost::apply_visitor( ActionVisitor{}, action );
                if( is_sync_with_client )
                {
                    ModelSyncServer::getInstance().sendAction( intermediate_result );
                }
            }
            catch( std::exception& e )
            {
                LOG(error) << e.what();
                //TODO: show error modal
            }

            action_queue_ptr->pop();
        }
    });
}

void Context::wait()
{
    if( loop_thread.joinable() )
    {
        loop_thread.join();
    }
}

void Context::stop()
{
    is_loop.store( false, std::memory_order_release );
    ActionVariant action{ SyncAction{ dummy_id, nullptr } };
    action_queue_ptr->push( std::make_tuple( std::move( action ), true ) );
    wait();
    action_queue_ptr->clear();
}

std::shared_ptr< Context::action_queue_type > Context::getQueuePtr() const
{
    return action_queue_ptr;
}

Context::Context()
    : action_queue_ptr( std::make_shared< action_queue_type>() ),
      is_loop( false )
{
}
