/*!==========================================================================
* \file
* - Program:       multiqueue
* - File:          MultiQueueProcessor.h
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
#ifndef __MULTITHREADPROCESSOR_H_63431C7A_ACF2_4A6F_AF3C_9870F10D2C43__
#define __MULTITHREADPROCESSOR_H_63431C7A_ACF2_4A6F_AF3C_9870F10D2C43__
//-------------------------------------------------------------------------//
#include <map>
#include <deque>
#include <atomic>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <functional>
//-------------------------------------------------------------------------//
#include "concurrency-queue.h"
#include "concurrency-map.h"
//-------------------------------------------------------------------------//
#define MAXCAPACITY 1000
//-------------------------------------------------------------------------//
namespace multiqueue
{
    template<typename Key, typename Value>
    struct IConsumer
    {
        using key_type = Key;
        using value_type = Value;

        virtual auto Consume(const Key & id, const Value & value) -> void = 0;
    };
//-------------------------------------------------------------------------//
    template<typename Key, typename Value>
    class MultiQueueProcessor final
    {
        using mutex_guard_t = std::unique_lock<std::mutex>;
        using consumers_t = concurrency::map<Key, IConsumer<Key, Value> *>;
        using deque_t = concurrency::queue<Value>;
        using queues_t = concurrency::map<Key, deque_t>;

    protected:
        //!< Keeps a map of consumers.
        consumers_t consumers;
        //!< Keeps a map of messages (key, messages).
        queues_t queues;
        //!< Keeps a flag of stopped or not.
        std::atomic_bool running;
        std::thread thread;

    public:
        /**
         * Constructor.
         */
        MultiQueueProcessor() : running(true), thread(std::bind(&MultiQueueProcessor::onthread, std::ref(*this)))
        {
        }

        /**
         * Destructor.
         * @throw None.
         */
        ~MultiQueueProcessor() noexcept
        {
            this->StopProcessing();

            if (this->thread.joinable() != false) { this->thread.join(); }
        }

        /**
         * Stops to proceed message.
         */
        auto StopProcessing() -> void
        {
            this->running = false;
        }

        /**
         * Adds a new subscriber to proceed.
         * @param key [in] - A unique key of subscriber.
         * @param consumer [in] - A consumer.
         */
        auto Subscribe(const Key & key, IConsumer<Key, Value> * consumer) -> void
        {
            assert(consumer != nullptr);

            if (this->consumers.contains(key) != true && consumer != nullptr)
            {
                this->consumers.insert({key, consumer});
            }
        }

        /**
         * Removes to support of subscriber.
         * @param key [in] - A key of subscriber.
         */
        void Unsubscribe(const Key & key)
        {
            this->consumers.erase(key);
        }

        /**
         * Adds a new message for subscriber.
         * @param id
         * @param value
         */
        void Enqueue(const Key & key, Value value)
        {
            try
            {
                auto & queue = this->queues.find(key);
                // Adding a new message into queue.
                queue.enqueue(std::move(value));
            }
            catch (const std::invalid_argument &)
            {// No data found.
                try
                {
                    deque_t que(MAXCAPACITY);
                    // Adding the value.
                    que.enqueue(std::move(value));
                    // Adding a new message into queue.
                    this->queues.insert({key, std::move(que)});
                }
                catch (const std::out_of_range & exc)
                {
                    std::clog << exc.what() << std::endl;
                }
            }
        }

        /**
         * Gets the first message from the queue of subscriber.
         * @param key [in] - A subscriber key or id.
         * @return A message.
         * @throw std::invalid_argument - No one message found.
         */
        auto Dequeue(const Key & key) -> Value
        {
            if (this->queues.empty() != true)
            {
                auto & queue = this->queues.find(key);

                if (queue.empty() != true)
                {
                    return std::move(queue.dequeue());
                }
            }
            throw (std::invalid_argument("No one message found"));
        }

        /**
         * Waits for finishing a thread.
         */
        auto Wait() -> void
        {
           if (this->thread.joinable()) { this->thread.join(); }
        }

        /**
         * Gets a count of messages in the queue.
         * @param key [in] - A key of consumer.
         * @return A count of messages.
         */
        auto Size(const Key & key) -> size_t
        {
            if (this->queues.contains(key) != false)
            {
                return this->queues.find(key).size();
            }
            return 0;
        }

    protected:
        //!< A thread function, which proceeds messages from queue.
        auto onthread() -> void
        {
            while (this->running != false)
            {
                try
                {
                    this->consumers.for_each([this](const typename consumers_t::value_type & consumer) -> void {

                        if (this->queues.empty() != true)
                        {
                            auto async = std::async(std::launch::async, [this, &consumer]() {

                                try
                                {
                                    // Getting a value.
                                    auto value = this->Dequeue(consumer.first);
                                    // Forwarding the message.
                                    consumer.second->Consume(consumer.first, value);
                                }
                                catch (const std::invalid_argument &)
                                {
                                }
                                catch (const std::exception & exc)
                                {
                                    std::cerr << "[ERROR] " << exc.what() << std::endl;
                                }
                            });
                        }
                    });
                }
                catch (const std::exception &)
                {
                }
            }
        }
    };
//-------------------------------------------------------------------------//
}; // namespace multiqueue
//-------------------------------------------------------------------------//
#endif // __MULTITHREADPROCESSOR_H_63431C7A_ACF2_4A6F_AF3C_9870F10D2C43__