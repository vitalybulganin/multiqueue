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
#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
//-------------------------------------------------------------------------//
#include "concurrency-queue.h"
#include "concurrency-consumer.h"
//-------------------------------------------------------------------------//
#define MaxCapacity 1000
//-------------------------------------------------------------------------//
namespace multiqueue
{
    template<typename Key, typename Value>
    struct IConsumer
    {
        virtual auto Consume(Key id, const Value & value) -> void = 0;
    };
//-------------------------------------------------------------------------//
    template<typename Key, typename Value>
    class MultiQueueProcessor final
    {
        using mutex_guard_t = std::unique_lock<std::mutex>;
        using concurrency_queue_t = multiqueue::concurrency_queue<Value>;

    protected:
        //!< Keeps a map of consumers.
        concurrency::map<Key, IConsumer<Key, Value> *> consumers;
        //!< Keeps a map of messages (key, messages).
        std::map<Key, concurrency_queue_t> queues;
        //!< Keeps a flag of stopped or not.
        volatile std::atomic_bool running;
        std::mutex lock;
        std::thread thread;

    public:
        /**
         * Constructor.
         */
        MultiQueueProcessor() : running(true), thread(std::bind(&MultiQueueProcessor::process, this))
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
         *
         * @param id
         * @param consumer
         */
        auto Subscribe(Key id, IConsumer<Key, Value> * consumer) -> void
        {
            mutex_guard_t sync(this->lock);

            auto iter = this->consumers.find(id);

            if (iter == this->consumers.end())
            {
                this->consumers.insert({id, consumer});
            }
        }

        /**
         *
         * @param id
         */
        void Unsubscribe(Key id)
        {
            mutex_guard_t sync(this->lock);

            auto iter = consumers.find(id);

            if (iter != consumers.end()) { this->consumers.erase(id); }
        }

        /**
         *
         * @param id
         * @param value
         */
        void Enqueue(Key id, Value value)
        {
            mutex_guard_t sync(this->lock);

            auto iter = queues.find(id);

            if (iter != queues.end())
            {
                if (iter->second.size() < MaxCapacity)
                    iter->second.push_back(value);
            }
            else
            {
                queues.insert(std::make_pair(id, std::list<Value>()));
                iter = queues.find(id);

                if (iter != queues.end())
                {
                    if (iter->second.size() < MaxCapacity)
                        iter->second.push_back(value);
                }
            }
        }

        /**
         *
         * @param id
         * @return
         */
        Value Dequeue(Key id)
        {
            mutex_guard_t sync(this->lock);

            auto iter = queues.find(id);

            if (iter != queues.end())
            {
                if (iter->second.size() > 0)
                {
                    auto front = iter->second.front();
                    iter->second.pop_front();
                    return front;
                }
            }
            return Value {};
        }

    protected:
        //!<
        auto process() -> void
        {
            while (this->running)
            {
                mutex_guard_t sync(this->lock);

                for (auto iter = queues.begin(); iter != queues.end(); ++iter)
                {
                    auto consumerIter = consumers.find(iter->first);

                    if (consumerIter != consumers.end())
                    {
                        Value front = Dequeue(iter->first);
                        if (front != Value {})
                            consumerIter->second->Consume(iter->first, front);
                    }
                }
            }
        }
    };
//-------------------------------------------------------------------------//
}; // namespace multiqueue
//-------------------------------------------------------------------------//
#endif // __MULTITHREADPROCESSOR_H_63431C7A_ACF2_4A6F_AF3C_9870F10D2C43__