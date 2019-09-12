/*!==========================================================================
* \file
* - Program:       multiqueue
* - File:          gtest-queue.h
* - Created:       11/23/2018
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
#ifndef __GTEST_QUEUE_H_B0A3730A_0088_4EF2_A420_61C39ED7FAF1__
#define __GTEST_QUEUE_H_B0A3730A_0088_4EF2_A420_61C39ED7FAF1__
//-------------------------------------------------------------------------//
#include <string>
#include <stdexcept>
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
    using queue_processor_t = multiqueue::MultiQueueProcessor<std::string, std::string>;

    std::vector<std::shared_ptr<consumer>> consumers = {
        std::shared_ptr<consumer>(new consumer()),
        std::shared_ptr<consumer>(new consumer())
    };
    queue_processor_t processor;
    std::thread producer1([](queue_processor_t & processor) -> void {
        for (auto i = 0; i < 100000000; ++i)
        {
            processor.Enqueue("1", std::to_string(i));
        }
    }, std::ref(processor));
    std::thread producer2([](queue_processor_t & processor) -> void {
        for (auto i = 0; i < 100000000; ++i)
        {
            processor.Enqueue("2", std::to_string(i + 100));
        }
    }, std::ref(processor));
    std::thread producer3([](queue_processor_t & processor) -> void {
        for (auto i = 0; i < 100000; ++i)
        {
            processor.Enqueue("10", std::to_string(i));
        }
    }, std::ref(processor));

    processor.Subscribe("1", consumers[0].get());
    processor.Subscribe("2", consumers[1].get());

    producer1.join();
    producer2.join();
    producer3.join();
}
//-------------------------------------------------------------------------//
#endif // __GTEST_QUEUE_H_B0A3730A_0088_4EF2_A420_61C39ED7FAF1__
