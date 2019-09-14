/*!==========================================================================
* \file
* - Program:       gtest-multiqueue
* - File:          gtest-queue.h
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
#ifndef __GTEST_QUEUE_H_B0A3730A_0088_4EF2_A420_61C39ED7FAF1__
#define __GTEST_QUEUE_H_B0A3730A_0088_4EF2_A420_61C39ED7FAF1__
//-------------------------------------------------------------------------//
#include <string>
#include <stdexcept>
#include <thread>
#include <atomic>
//-------------------------------------------------------------------------//
#include <gtest/gtest.h>
//-------------------------------------------------------------------------//
#include "../../concurrency-queue.h"
//-------------------------------------------------------------------------//
TEST(TestQueue, empty)
{
    auto queue = multiqueue::concurrency::queue<std::string>();
    ASSERT_TRUE(queue.empty());
    queue.enqueue("message 1");
    ASSERT_FALSE(queue.empty());
}

TEST(TestQueue, size)
{
    auto queue = multiqueue::concurrency::queue<std::string>();
    ASSERT_TRUE(queue.size() == 0);
    queue.enqueue("message 1");
    ASSERT_TRUE(queue.size() == 1);
}

TEST(TestQueue, enqueue)
{
    auto queue = multiqueue::concurrency::queue<std::string>();

    std::thread thread1([&queue]() {
        for (auto i = 0; i < 10; ++i)
        {
            ASSERT_NO_THROW(queue.enqueue("message 1"));
        }
    });

    std::thread thread2([&queue]() {
        for (auto i = 0; i < 10; ++i)
        {
            ASSERT_NO_THROW(queue.enqueue("message 2"));
        }
    });
    thread1.join();
    thread2.join();
    ASSERT_TRUE(queue.size() == 20);
}

TEST(TestQueue, dequeueSync)
{
    auto queue = multiqueue::concurrency::queue<std::string>();

    std::thread thread1([&queue]() {
        for (auto i = 0; i < 10; ++i)
        {
            ASSERT_NO_THROW(queue.enqueue("message 1"));
        }
    });

    std::thread thread2([&queue]() {
        for (auto i = 0; i < 10; ++i)
        {
            ASSERT_NO_THROW(queue.enqueue("message 2"));
        }
    });
    thread1.join();
    thread2.join();
    ASSERT_TRUE(queue.size() == 20);
    while (queue.empty() != true)
    {
        ASSERT_NO_THROW(queue.dequeue());
    }
    ASSERT_TRUE(queue.size() == 0);
    ASSERT_THROW(queue.dequeue(), std::out_of_range);
}
TEST(TestQueue, dequeueAsync)
{
    std::atomic_int count(0);
    auto queue = multiqueue::concurrency::queue<std::string>();

    std::thread thread1([&queue]() {
        for (auto i = 0; i < 10; ++i)
        {
            ASSERT_NO_THROW(queue.enqueue("message 1"));
        }
    });

    std::thread thread2([&queue]() {
        for (auto i = 0; i < 10; ++i)
        {
            ASSERT_NO_THROW(queue.enqueue("message 2"));
        }
    });

    std::thread thread3([&queue, &count]() {
        while (count != 20)
        {
            if (queue.empty() != true) { ASSERT_NO_THROW(queue.dequeue()); ++count; }
        }
    });
    thread1.join();
    thread2.join();
    thread3.join();

    ASSERT_TRUE(queue.size() == 0);
    ASSERT_TRUE(count == 20);
    ASSERT_THROW(queue.dequeue(), std::out_of_range);
}
//-------------------------------------------------------------------------//
#endif // __GTEST_QUEUE_H_B0A3730A_0088_4EF2_A420_61C39ED7FAF1__
