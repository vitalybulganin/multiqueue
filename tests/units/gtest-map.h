/*!==========================================================================
* \file
* - Program:       gtest-multiqueue
* - File:          gtest-map.h
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
#ifndef __GTEST_MAP_H_03084F3E_47F8_4369_97B8_80E61A679901__
#define __GTEST_MAP_H_03084F3E_47F8_4369_97B8_80E61A679901__
//-------------------------------------------------------------------------//
#include <string>
#include <stdexcept>
#include <thread>
#include <atomic>
//-------------------------------------------------------------------------//
#include <gtest/gtest.h>
//-------------------------------------------------------------------------//
#include "../../concurrency-map.h"
//-------------------------------------------------------------------------//
TEST(TestMap, empty)
{
    multiqueue::concurrency::map<int, std::string> map;
    ASSERT_TRUE(map.empty());
    map.insert({1, "message 1"});
    ASSERT_FALSE(map.empty());
}

TEST(TestMap, size)
{
    multiqueue::concurrency::map<int, std::string> map;
    ASSERT_TRUE(map.size() == 0);
    map.insert({1, "message 1"});
    ASSERT_TRUE(map.size() == 1);
}

TEST(TestMap, insert)
{
    multiqueue::concurrency::map<int, std::string> map;

    std::thread thread1([&map]() {
        for (auto i = 0; i < 10; ++i)
        {
            ASSERT_NO_THROW(map.insert({i, "message 1"}));
        }
    });

    std::thread thread2([&map]() {
        for (auto i = 10; i < 20; ++i)
        {
            ASSERT_NO_THROW(map.insert({i, "message 2"}));
        }
    });
    thread1.join();
    thread2.join();
    ASSERT_TRUE(map.size() == 20);
}

TEST(TestMap, find)
{
    multiqueue::concurrency::map<int, std::string> map;

    std::thread thread1([&map]() {
        for (auto i = 0; i < 10; ++i)
        {
            ASSERT_NO_THROW(map.insert({i, "message 1"}));
        }
    });

    std::thread thread2([&map]() {
        for (auto i = 10; i < 20; ++i)
        {
            ASSERT_NO_THROW(map.insert({i, "message 2"}));
        }
    });
    thread1.join();
    thread2.join();
    ASSERT_TRUE(map.size() == 20);
    for (auto i = 0; i < 20; ++i)
    {
        ASSERT_NO_THROW(map.find(i));
    }
    ASSERT_TRUE(map.find(1) == "message 1");
    ASSERT_TRUE(map.find(10) == "message 2");
    ASSERT_THROW(map.find(20), std::invalid_argument);
}
TEST(TestMap, contains)
{
    multiqueue::concurrency::map<int, std::string> map;

    ASSERT_NO_THROW(map.insert({1, "message 1"}));
    ASSERT_TRUE(map.contains(1));
    ASSERT_FALSE(map.contains(2));
}
//-------------------------------------------------------------------------//
#endif // __GTEST_MAP_H_03084F3E_47F8_4369_97B8_80E61A679901__
