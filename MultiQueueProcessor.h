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
        using concurrency_queue_t = multiqueue::concurrency_queue<Value>;
        using consumers_t = concurrency::map<Key, IConsumer<Key, Value> *>;
        using deque_t = std::deque<Value>;
        using queues_t = std::map<Key, deque_t>;

    protected:
        //!< Keeps a map of consumers.
        consumers_t consumers;
        //!< Keeps a map of messages (key, messages).
        queues_t queues;
        //!< Keeps a flag of stopped or not.
        std::atomic_bool running;
        mutable std::mutex lock;
        std::condition_variable condition;
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
            this->condition.notify_all();
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
            mutex_guard_t sync(this->lock);
            auto iter = this->queues.find(key);

            if (iter != this->queues.end())
            {
                if ((*iter).second.size() < MAXCAPACITY) { (*iter).second.push_back(std::move(value)); this->condition.notify_one(); }
            }
            else
            {
                deque_t que;
                // Adding the value.
                que.push_back(std::move(value));
                // Adding a new message into queue.
                this->queues.insert({key, std::move(que)});

                this->condition.notify_one();
            }
        }

        /**
         *
         * @param id
         * @return
         */
        auto Dequeue(const Key & key) -> Value
        {
            Value value;
            mutex_guard_t sync(this->lock);

            if (this->queues.empty() != true)
            {
                auto iter = this->queues.find(key);

                if (iter != this->queues.end() && (*iter).second.empty() != true)
                {
                    value = std::move((*iter).second.front());
                    // Removing the first element.
                    (*iter).second.pop_front();
                }
            }
            return std::move(value);
        }

        auto Empty() const -> bool
        {
            mutex_guard_t sync(this->lock);

            return this->queues.empty();
        }

    protected:
        //!<
        auto onthread() -> void
        {
            while (this->running != false)
            {
                try
                {
                    this->consumers.for_each([this](const typename consumers_t::value_type & consumer) -> void {

                        if (this->Empty() != true)
                        {
                            auto async = std::async(std::launch::async, [this, &consumer]() {
                                try
                                {
                                    // Getting a value.
                                    auto value = this->Dequeue(consumer.first);
                                    // Forwarding the message.
                                    consumer.second->Consume(consumer.first, value);
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