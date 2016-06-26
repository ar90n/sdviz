#ifndef __SDVIZ_CONTEXT_HPP__
# define __SDVIZ_CONTEXT_HPP__

# include <memory>
# include <thread>
# include <atomic>
# include <tuple>

# include "action.hpp"
# include "mpsc_queue.hpp"

namespace sdviz
{

    class Context final
    {
        public:
            using action_queue_type = MPSCQueue< std::tuple< ActionVariant, bool > >;

            Context( Context const& _context ) = delete;
            Context( Context&& _context ) = delete;
            ~Context() = default;

            Context& operator=( Context const& _context ) = delete;
            Context& operator=( Context&& _context ) = delete;

            static Context& getInstance();
            void start();
            void wait();
            void stop();
            std::shared_ptr< action_queue_type> getQueuePtr() const;

        private:
            Context();

            std::shared_ptr< action_queue_type > action_queue_ptr;
            std::thread loop_thread;
            std::atomic< bool > is_loop;
    };
}

#endif //__SDVIZ_CONTEXT_HPP__
