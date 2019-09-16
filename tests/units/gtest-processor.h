/*!==========================================================================
* \file
* - Program:       gtest-multiqueue
* - File:          gtest-processor.h
* - Created:       09/14/2019
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
#ifndef __GTEST_PROCESSOR_H_EEE5A9DC_9450_4945_988B_EE8DFEDB6ABC__
#define __GTEST_PROCESSOR_H_EEE5A9DC_9450_4945_988B_EE8DFEDB6ABC__
//-------------------------------------------------------------------------//
#include <string>
#include <stdexcept>
#include <atomic>
//-------------------------------------------------------------------------//
#include <gtest/gtest.h>
//-------------------------------------------------------------------------//
#include "../../MultiQueueProcessor.h"
//-------------------------------------------------------------------------//
namespace
{
    class consumer : public multiqueue::IConsumer<std::string, std::string>
    {
        using base_class = multiqueue::IConsumer<std::string, std::string>;

    public:
        virtual auto Consume(const base_class::key_type & id, const base_class::value_type & value) -> void override
        {
            std::clog << "Key = " << id << ", value = " << value << std::endl;
        }
    };
}; // namespace
//-------------------------------------------------------------------------//
TEST(TestQueue, multiqueue)
{
    std::atomic_int count(0);
    using queue_processor_t = multiqueue::MultiQueueProcessor<std::string, std::string>;

    std::vector<std::shared_ptr<consumer>> consumers = {
        std::shared_ptr<consumer>(new consumer()),
        std::shared_ptr<consumer>(new consumer())
    };
    queue_processor_t processor;

    processor.Subscribe("1", consumers[0].get());
    processor.Subscribe("2", consumers[1].get());
    processor.Subscribe("3", consumers[1].get());

    std::thread producer1([](queue_processor_t & processor) -> void {
        for (auto i = 0; i < 1000; ++i)
        {
            processor.Enqueue("1", std::to_string(i));
        }
    }, std::ref(processor));
    std::thread producer2([](queue_processor_t & processor) -> void {
        for (auto i = 0; i < 1000; ++i)
        {
            processor.Enqueue("2", std::to_string(i));
        }
    }, std::ref(processor));
    std::thread manager([](queue_processor_t & processor) -> void {
        static std::string s_keys[2] = {"1", "2"};

        while (true)
        {
            size_t count = 0;

            for (auto i = 0; i < 2; ++i)
            {
                count += processor.Size(s_keys[i]);
            }
            if (count == 0) { processor.StopProcessing(); break; }
        }
    }, std::ref(processor));

    producer1.join();
    producer2.join();

    processor.Wait();
    manager.join();
}
//-------------------------------------------------------------------------//
#endif // __GTEST_PROCESSOR_H_EEE5A9DC_9450_4945_988B_EE8DFEDB6ABC__
