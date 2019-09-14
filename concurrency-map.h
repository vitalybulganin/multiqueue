/*!==========================================================================
* \file
* - Program:       multiqueue
* - File:          concurrency-consumer.h
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
#ifndef __CONCURRENCY_CONSUMER_H_5460DAB4_02F0_4A8B_A618_A4046C88D84E__
#define __CONCURRENCY_CONSUMER_H_5460DAB4_02F0_4A8B_A618_A4046C88D84E__
//-------------------------------------------------------------------------//
#include <map>
#include <mutex>
//-------------------------------------------------------------------------//
namespace multiqueue
{
    namespace concurrency
    {
//-------------------------------------------------------------------------//
        template<typename Key, typename Value>
        class map final
        {
            using mutex_lock_t = std::unique_lock<std::mutex>;
            using objects_t = std::map<Key, Value>;
            using iterator = typename objects_t::iterator;
            //!< Keeps a map of objects.
            objects_t objects;
            //!< Keeps a mutex.
            mutable std::mutex lock;

        public:
            using value_type = typename std::map<Key, Value>::value_type;

        public:
            map(const map &) = delete;
            map(const map &&) = delete;
            auto operator=(const map &) -> map = delete;

        public:
            auto operator[](const Key & key) -> Value &
            {
                return this->find(key).second;
            }

        public:
            //!< Constructor.
            map() = default;

            /**
             * Destructor.
             * @throw None.
             */
            ~map() noexcept = default;

            /**
             *
             * @param value
             */
            auto insert(const std::pair<Key, Value> & value) -> void
            {
                iterator iter;
                {
                    mutex_lock_t sync(this->lock);
                    iter = this->objects.find(value.first);
                }
                if (iter == this->objects.end())
                {
                    {
                        mutex_lock_t sync(this->lock);

                        this->objects.insert(value);
                    }
                }
            }

            /**
             *
             * @param key
             */
            auto erase(const Key & key) -> void
            {
                mutex_lock_t sync(this->lock);

                this->objects.erase(key);
            }

            /**
             *
             * @return
             */
            auto empty() const
            {
                mutex_lock_t sync(this->lock);

                return this->objects.empty();
            }

            /**
             *
             * @return
             */
            auto size() const
            {
                mutex_lock_t sync(this->lock);

                return this->objects.size();
            }

            /**
             *
             * @param key
             * @return
             */
            auto find(const Key & key) -> Value &
            {
                mutex_lock_t sync(this->lock);

                auto iter = this->objects.find(key);

                if (iter != this->objects.end()) { return (*iter).second; }

                throw (std::invalid_argument("No one key found."));
            }

            /**
             *
             * @param key
             * @return
             */
            auto find(const Key & key) const -> const Value &
            {
                return this->find(key);
            }

            /**
             *
             * @param key
             * @return
             */
            auto contains(const Key & key) const -> bool
            {
                mutex_lock_t sync(this->lock);

                return this->objects.find(key) != this->objects.end();
            }

            /**
             *
             * @param callback
             */
            auto for_each(const std::function<void(const value_type & value)> & callback) -> void
            {
                iterator begin, end;
                {
                    mutex_lock_t sync(this->lock);
                    // Getting the first element.
                    begin = this->objects.begin();
                    end = this->objects.end();
                }
                for (; begin != end; ++begin)
                {
                    callback(*begin);
                }
            }
        };
//-------------------------------------------------------------------------//
    }; // namespace concurrency
}; // namespace multiqueue
//-------------------------------------------------------------------------//
#endif // __CONCURRENCY_CONSUMER_H_5460DAB4_02F0_4A8B_A618_A4046C88D84E__
