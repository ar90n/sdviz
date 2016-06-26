#ifndef __SDVIZ_MPSC_QUEUE_HPP__
# define __SDVIZ_MPSC_QUEUE_HPP__

# include <queue>
# include <mutex>
# include <condition_variable>

namespace sdviz
{
    template< typename ValueType >
    class MPSCQueue
    {
        public:
            using value_type = ValueType;

            MPSCQueue() = default;
            MPSCQueue( MPSCQueue const& _queue ) = delete;
            MPSCQueue( MPSCQueue&& _queue ) = default;
            ~MPSCQueue() = default;

            MPSCQueue& operator =( MPSCQueue const& _queue ) = delete;
            MPSCQueue& operator =( MPSCQueue&& _queue ) = default;

            value_type& front();
            void pop();
            void push( value_type&& item );
            int size();
            void clear();

        private:
            std::queue< value_type > queue;
            std::mutex mutex;
            std::condition_variable cond;
    };

    template< typename ValueType >
    typename MPSCQueue< ValueType >::value_type& MPSCQueue< ValueType >::front()
    {
        std::unique_lock< std::mutex > mlock( mutex );
        while( queue.empty() )
        {
            cond.wait( mlock );
        }

        return queue.front();
    }

    template< typename ValueType >
    void MPSCQueue< ValueType >::pop()
    {
        std::unique_lock< std::mutex > mlock( mutex );
        while( queue.empty() )
        {
            cond.wait( mlock );
        }

        queue.pop();
    }

    template< typename ValueType >
    void MPSCQueue< ValueType >::push( value_type&& item )
    {
        std::unique_lock< std::mutex > mlock( mutex );
        queue.push( std::move( item ) );
        mlock.unlock();
        cond.notify_one();
    }

    template< typename ValueType >
    int MPSCQueue< ValueType >::size()
    {
        std::unique_lock< std::mutex > mlock( mutex );
        int size = queue.size();
        mlock.unlock();

        return size;
    }

    template< typename ValueType >
    void MPSCQueue< ValueType >::clear()
    {
        std::unique_lock< std::mutex > mlock( mutex );
        std::queue< value_type >().swap( queue );
        mlock.unlock();
    }
}

#endif // __SDVIZ_MPSC_QUEUE_HPP__
