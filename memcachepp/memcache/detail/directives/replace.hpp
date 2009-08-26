#ifndef MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_REPLACE_HPP_
#define MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_REPLACE_HPP_

// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

namespace memcache { namespace detail {

    template <class T>
        struct replace_directive {
            
            explicit replace_directive(std::string const & key, T const & value, boost::uint16_t flags, time_t timeout, time_t failover_timeout) :
                key(key), value(value), flags(flags), timeout(timeout), failover_timeout(failover_timeout) 
            { };
            
            template <class Handle>
                void operator() (Handle & handle) const {
                    handle.replace(
                            handle.hash(
                                key, 
                                handle.pool_count()
                                ), 
                            key, 
                            value, 
                            timeout, 
                            failover_timeout, 
                            flags);
                }

        private:
            std::string key;
            T const & value;
            boost::uint16_t flags;
            time_t timeout;
            time_t failover_timeout;
        };

} // namespace detail


    template <typename T, typename _T>
	inline detail::replace_directive<_T> replace(T _key, _T const & value, time_t timeout = 0, boost::uint16_t flags = 0) {
        return detail::replace_directive<_T>(std::string(_key), value, flags, timeout, timeout);
    };

    template <typename T, typename _T>
	inline detail::replace_directive<_T> replace(T _key, _T const & value, detail::expire_type const & expiration, detail::failover_expire_type const & failover_expiration, boost::uint16_t flags = 0) {
        return detail::replace_directive<_T>(std::string(_key), value, flags, expiration.timeout, failover_expiration.timeout);
    };

    template <typename T, typename _T>
	inline detail::replace_directive<_T> replace(T _key, _T const & value, detail::failover_expire_type const & failover_expiration, boost::uint16_t flags = 0) {
        return detail::replace_directive<_T>(std::string(_key), value, flags, 0, failover_expiration.timeout);
    };

    template <typename T, typename _T>
	inline detail::replace_directive<_T> replace(T _key, _T const & value, detail::failover_expire_type const & failover_expiration, detail::expire_type const & expiration, boost::uint16_t flags = 0) {
        return detail::replace_directive<_T>(std::string(_key), value, flags, expiration.timeout, failover_expiration.timeout);
    };

} // namespace memcache

#endif


