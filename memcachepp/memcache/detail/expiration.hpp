
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEMCACHE_DETAIL_EXPIRATION_HPP__
#define MEMCACHE_DETAIL_EXPIRATION_HPP__

namespace memcache { namespace detail {

    struct expire_type {
        time_t timeout;
    };

    struct failover_expire_type {
        time_t timeout;
    };

} // namespace detail

    template <typename T>
    inline detail::expire_type expire(T const & timeout) {
        BOOST_STATIC_ASSERT((boost::is_integral<T>::value));
        detail::expire_type result = { boost::numeric_cast<time_t>(timeout) };
        return result;
    }

    template <typename T>
    inline detail::failover_expire_type failover_expire(T const & timeout) {
        BOOST_STATIC_ASSERT((boost::is_integral<T>::value));
        detail::failover_expire_type result = { boost::numeric_cast<time_t>(timeout) };
        return result;
    }

} // namespace memcache

#endif // MEMCACHE_DETAIL_EXPIRATION_HPP__

