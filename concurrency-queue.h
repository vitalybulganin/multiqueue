/*!==========================================================================
* \file
* - Program:       multiqueue
* - File:          concurrency-queue.h
* - Created:       09/11/2019
* - Author:        Vitaly Bulganin
* - Description:
* - Comments:
*
-----------------------------------------------------------------------------
*
* - History:
*
===========================================================================*/
#pragma once
//-------------------------------------------------------------------------//
#ifndef __CONCURRENCY_QUEUE_H_C46D5A08_CE49_4086_9105_1372D71F64D5__
#define __CONCURRENCY_QUEUE_H_C46D5A08_CE49_4086_9105_1372D71F64D5__
//-------------------------------------------------------------------------//
#include <vector>
#include <mutex>
#include <atomic>
//-------------------------------------------------------------------------//
namespace multiqueue
{
//-------------------------------------------------------------------------//
    template<typename TMessage>
    class concurrency_queue final
    {
        using mutex_guard_t = std::lock_guard<std::mutex>;
        //!< Keeps max count of messages.
        std::atomic<size_t> capacity;
        //!< Keeps a list of messages.
        std::vector<TMessage> queue;
        //!< Keeps a mutex.
        mutable std::mutex lock;

    public:
        /**
         * Constructor.
         */
        concurrency_queue() : capacity(1000), queue(capacity * sizeof(TMessage))
        {
        }

        /**
         * Constructor.
         * @param maxcount [in] - A max count of messages in the queue.
         */
        concurrency_queue(const size_t & maxcount) : capacity(maxcount), queue(capacity * sizeof(TMessage))
        {
        }

        /**
         * Destructor.
         * @throw None.
         */
        ~concurrency_queue() noexcept = default;

        /**
         * Adds a new message into queue.
         * @param message [in] - A new message.
         */
        auto push(const TMessage & message) -> void
        {
            if (this->size() >= this->capacity) { throw (std::out_of_range("Too many messages in the queue.")); }

            mutex_guard_t sync(this->lock);
            // Adding a message into collection.
            this->queue.push_back(message);
        }

        /**
         * Gets the first message from the queue and removes it.
         * @return The first message.
         * @throw std::out_of_range - No one message found.
         */
        auto pop() -> TMessage
        {
            if (this->empty() != false) { throw (std::out_of_range("No one message found.")); }

            mutex_guard_t sync(this->lock);

            auto message = *this->queue.begin();
            this->queue.erase(this->queue.begin());

            return std::move(message);
        }

        /**
         * Checks the queue on empty.
         * @return true, if the queue is empty, otherwise false.
         */
        auto empty() const -> bool
        {
            mutex_guard_t sync(this->lock);

            return this->queue.empty();
        }

        /**
         * Getts a count of messages in the queue.
         * @return A count of messages.
         */
        auto size() const -> size_t
        {
            mutex_guard_t sync(this->lock);

            return this->queue.size();
        }

        /**
         * Gets a max count of keeping messages.
         * @param size [in] - A count of messages.
         */
        auto maxsize(const size_t & size) -> void
        {
            this->capacity = size;

            mutex_guard_t sync(this->lock);

            this->queue.reserve(size * sizeof(TMessage));
        }

        /**
         * Gets max count of keeping messages.
         * @return A max count of messages.
         */
        auto maxsize() const -> size_t
        {
            return this->capacity;
        }
    };
//-------------------------------------------------------------------------//
}; // namespace multiqueue
//-------------------------------------------------------------------------//
#endif // __CONCURRENCY_QUEUE_H_C46D5A08_CE49_4086_9105_1372D71F64D5__
