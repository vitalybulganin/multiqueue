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
#include <deque>
#include <mutex>
#include <condition_variable>
#include <memory>
//-------------------------------------------------------------------------//
namespace multiqueue
{
    namespace concurrency
    {
//-------------------------------------------------------------------------//
        template<typename TMessage>
        class queue final
        {
            using mutex_guard_t = std::unique_lock<std::mutex>;
            //!< Keeps a queue capacity.
            const size_t capacity = 0;
            //!< Keeps a list of messages.
            std::deque<TMessage> messages;
            //!< Keeps a mutex.
            mutable std::shared_ptr<std::mutex> lock;
            mutable std::shared_ptr<std::condition_variable> cond;

        public:
            /**
             * Constructor.
             */
            queue() : lock(new std::mutex()), cond(new std::condition_variable())
            {
            }

            /**
             * Constructor.
             * @param maxcount [in] - A max count of messages in the queue.
             */
            queue(const size_t & maxcount) : capacity(maxcount), lock(new std::mutex()), cond(new std::condition_variable())
            {
            }

            /**
             * Destructor.
             * @throw None.
             */
            ~queue() noexcept = default;

            /**
             * Adds a new message into queue.
             * @param message [in] - A new message.
             */
            auto enqueue(const TMessage & message) -> void
            {
                mutex_guard_t sync(*this->lock);

                if (this->capacity > 0 && this->messages.size() >= this->capacity)
                {
                    while (this->cond->wait_for(sync, std::chrono::microseconds(10)) != std::cv_status::timeout)
                    {
                    }
                    //<????> throw (std::out_of_range("Too many messages in the queue [" + std::to_string(this->size()) + "]"));
                }
                // Adding a message into collection.
                this->messages.push_back(message);
            }

            /**
             * Gets the first message from the queue and removes it.
             * @return The first message.
             * @throw std::out_of_range - No one message found.
             */
            auto dequeue() -> TMessage
            {
                if (this->empty() != false) { throw (std::out_of_range("No one message found.")); }

                TMessage object;
                {
                    mutex_guard_t sync(*this->lock);
                    // Getting the first element.
                    object = std::move(this->messages.front());
                    // Removing the first element.
                    this->messages.pop_front();
                    //
                    this->cond->notify_one();
                }
                return std::move(object);
            }

            /**
             * Checks the queue on empty.
             * @return true, if the queue is empty, otherwise false.
             */
            auto empty() const -> bool
            {
                mutex_guard_t sync(*this->lock);

                return this->messages.empty();
            }

            /**
             * Getts a count of messages in the queue.
             * @return A count of messages.
             */
            auto size() const -> size_t
            {
                mutex_guard_t sync(*this->lock);

                return this->messages.size();
            }
        };
//-------------------------------------------------------------------------//
    }; // namespace concurrency
}; // namespace multiqueue
//-------------------------------------------------------------------------//
#endif // __CONCURRENCY_QUEUE_H_C46D5A08_CE49_4086_9105_1372D71F64D5__
