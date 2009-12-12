#ifndef MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_RAW_PREPEND_HPP_
#define MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_RAW_PREPEND_HPP_

// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

namespace memcache {

    namespace detail {

        template <class tag = tags::default_tag >
        struct raw_prepend_directive {
            explicit raw_prepend_directive(std::string const & key, std::string const & value, boost::uint16_t flags, time_t timeout, time_t failover_timeout) :
                key(key), value(value), flags(flags), timeout(timeout), failover_timeout(failover_timeout) { };

            template <class Handle>
            void operator() (Handle & handle) const {
                handle.prepend_raw(
                        handle.hash(
                            key, 
                            handle.pool_count()
                            ), 
                        key, 
                        value, 
                        timeout, 
                        failover_timeout, 
                        flags
                        );
            };

            private:

            std::string key;
            std::string value;
            boost::uint16_t flags;
            time_t timeout;
            time_t failover_timeout;
        };
    } // namespace detail

    template <class T>
    inline detail::raw_prepend_directive<> raw_prepend(T _key, std::string const & value, time_t timeout=0, boost::uint16_t flags = 0) {
        return detail::raw_prepend_directive<>(std::string(_key), value, flags, timeout, timeout);
    }

    template <class T>
    inline detail::raw_prepend_directive<> raw_prepend(T _key, std::string const & value, detail::expire_type const & expiration, detail::failover_expire_type const & failover_expiration, boost::uint16_t flags = 0) {
        return detail::raw_prepend_directive<>(std::string(_key), value, flags, expiration.timeout, failover_expiration.timeout);
    }

    template <class T>
    inline detail::raw_prepend_directive<> raw_prepend(T _key, std::string const & value, detail::expire_type const & expiration, boost::uint16_t flags = 0) {
        return detail::raw_prepend_directive<>(std::string(_key), value, flags, expiration.timeout, expiration.timeout);
    }

    template <class T>
    inline detail::raw_prepend_directive<> raw_prepend(T _key, std::string const & value, detail::failover_expire_type const & failover_expiration, boost::uint16_t flags = 0) {
        return detail::raw_prepend_directive<>(std::string(_key), value, flags, 0, failover_expiration.timeout);
    }

    template <class T>
    inline detail::raw_prepend_directive<> raw_prepend(T _key, std::string const & value, detail::failover_expire_type const & failover_expiration, detail::expire_type const & expiration, boost::uint16_t flags = 0) {
        return detail::raw_prepend_directive<>(std::string(_key), value, flags, expiration.timeout, failover_expiration.timeout);
    }

} // namespace memcache

#endif



