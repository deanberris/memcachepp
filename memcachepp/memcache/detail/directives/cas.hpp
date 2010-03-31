
// Copyright 2010 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_CAS_HPP_20100331
#define MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_CAS_HPP_20100331

#include <memcachepp/memcache/detail/expiration.hpp>
#include <boost/cstdint.hpp>

namespace memcache { namespace detail {
    template <class T>
    struct cas_directive {
        explicit cas_directive(std::string const & key, T const & value, boost::uint16_t flags, boost::uint64_t cas_value, time_t timeout, time_t failover_timeout) :
        key_(key), value_(value), flags_(flags), cas_value_(cas_value),
        timeout_(timeout), failover_timeout_(failover_timeout)
        {}

        template <class _T>
        void operator() (_T & handle) const {
            handle.check_and_set(
                handle.hash(
                    key_,
                    handle.pool_count()
                    ),
                key_,
                value_,
                cas_value_,
                timeout_,
                failover_timeout_,
                flags_);
        }

        private:

        std::string key_;
        T const & value_;
        boost::uint16_t flags_;
        boost::uint64_t cas_value_;
        time_t timeout_;
        time_t failover_timeout_;
    };
    
} /* detail */

template <class T, class _T>
inline detail::cas_directive<_T>
cas(T key, _T const & value, boost::uint64_t cas_value, time_t timeout = 0, boost::uint16_t flags = 0) {
    return detail::cas_directive<_T>(std::string(key), value, flags, cas_value, timeout, timeout);
}

template <class T, class _T>
inline detail::cas_directive<_T>
cas(T key, _T const & value, boost::uint64_t cas_value, detail::expire_type const & expiration, detail::failover_expire_type const & failover_expiration, boost::uint16_t flags = 0) {
    return detail::cas_directive<_T>(std::string(key), value, flags, cas_value, expiration.timeout, failover_expiration.timeout);
}

template <class T, class _T>
inline detail::cas_directive<_T>
cas(T key, _T const & value, boost::uint64_t cas_value, detail::failover_expire_type const & failover_expiration, boost::uint16_t flags = 0) {
    return detail::cas_directive<_T>(std::string(key), value, flags, cas_value, 0, failover_expiration.timeout);
}

template <class T, class _T>
inline detail::cas_directive<_T>
cas(T key, _T const & value, boost::uint64_t cas_value, detail::expire_type const & expiration, boost::uint16_t flags = 0) {
    return detail::cas_directive<_T>(std::string(key), value, flags, cas_value, expiration.timeout, 0, flags);
}

template <class T, class _T>
inline detail::cas_directive<_T>
cas(T key, _T const & value, boost::uint64_t cas_value, detail::failover_expire_type const & failover_expiration, detail::expire_type const & expiration, boost::uint16_t flags = 0) {
    return detail::cas_directive<_T>(std::string(key), value, flags, cas_value, expiration.timeout, failover_expiration.timeout);
}

} /* memcache */

#endif /* MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_CAS_HPP_20100331 */
