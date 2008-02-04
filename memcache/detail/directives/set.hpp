
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_SET_HPP__
#define __MEMACAHE_DETAIL_DIRECTIVES_SET_HPP__

namespace memcache { namespace detail {

    template <typename T>
    struct set_directive {
        explicit set_directive(std::string const & key, T const & value, uint16_t flags, time_t timeout, time_t failover_timeout) :
            _key(key), _value(value), _flags(flags), _timeout(timeout), _failover_timeout(failover_timeout) { };
        
        template <typename _T>
        void operator() (_T & handle) const {
            handle.set(
                    handle.hash(
                        _key, 
                        handle.pool_count()
                        ), 
                    _key, 
                    _value, 
                    _timeout, 
                    _failover_timeout, 
                    _flags);
        };

        private:

        mutable std::string _key;
        mutable T const & _value;
        mutable uint16_t _flags;
        mutable time_t _timeout;
        mutable time_t _failover_timeout;
    };

}; // namespace detail

}; // namespace memcache

#endif // __MEMACAHE_DETAIL_DIRECTIVES_SET_HPP__

