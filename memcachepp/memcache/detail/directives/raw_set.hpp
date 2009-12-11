
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_RAW_SET_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_RAW_SET_HPP__

#include <memcachepp/memcache/detail/expiration.hpp>
#include <memcachepp/memcache/detail/tags.hpp>

namespace memcache { namespace detail {

        template <class tag = tags::default_tag >
        struct raw_set_directive {
            explicit raw_set_directive(std::string const & key, std::string const & value, boost::uint16_t flags, time_t timeout, time_t failover_timeout) :
                _key(key), _value(value), _flags(flags), _timeout(timeout), _failover_timeout(failover_timeout) { };

            template <typename _T>
            void operator() (_T & handle) const {
                handle.set_raw(
                        handle.hash(
                            _key, 
                            handle.pool_count()
                            ), 
                        _key, 
                        _value, 
                        _timeout, 
                        _failover_timeout, 
                        _flags
                        );
            };

            private:

            mutable std::string _key;
            mutable std::string _value;
            mutable boost::uint16_t _flags;
            mutable time_t _timeout;
            mutable time_t _failover_timeout;
        };

}; // namespace detail

    template <class T>
    inline detail::raw_set_directive<> raw_set(T _key, std::string const & value, time_t timeout=0, time_t failover_timeout=0, boost::uint16_t flags = 0) {
        return detail::raw_set_directive<>(std::string(_key), value, flags, timeout, failover_timeout);
    };

    template <class T>
    inline detail::raw_set_directive<> raw_set(T _key, std::string const & value, detail::expire_type const & expiration, boost::uint16_t flags = 0) {
        return detail::raw_set_directive<>(std::string(_key), value, flags, expiration.timeout, expiration.timeout);
    }

    template <class T>
    inline detail::raw_set_directive<> raw_set(T _key, std::string const & value, detail::expire_type const & expiration, detail::failover_expire_type const & failover_expiration, boost::uint16_t flags = 0) {
        return detail::raw_set_directive<>(std::string(_key), value, flags, expiration.timeout, failover_expiration.timeout);
    }

    template <class T>
    inline detail::raw_set_directive<> raw_set(T _key, std::string const & value, detail::failover_expire_type const & failover_expiration, boost::uint16_t flags = 0) {
        return detail::raw_set_directive<>(std::string(_key), value, flags, 0, failover_expiration.timeout);
    }

    template <class T>
    inline detail::raw_set_directive<> raw_set(T _key, std::string const & value, detail::failover_expire_type const & failover_expiration, detail::expire_type const & expiration, boost::uint16_t flags = 0) {
        return detail::raw_set_directive<>(std::string(_key), value, flags, expiration.timeout, failover_expiration.timeout);
    }

}; // namespace memcache

#endif // __MEMCACHE_DETAIL_DIRECTIVES_RAW_SET_HPP__

