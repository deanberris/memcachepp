#ifndef MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_RAW_REPLACE_HPP_
#define MEMCACHEPP_MEMCACHE_DETAIL_DIRECTIVES_RAW_REPLACE_HPP_

// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

namespace memcache {

    namespace detail {

        template <class tag = tags::default_tag >
        struct raw_replace_directive {
            explicit raw_replace_directive(std::string const & key, std::string const & value, boost::uint16_t flags, time_t timeout, time_t failover_timeout) :
                key(key), value(value), flags(flags), timeout(timeout), failover_timeout(failover_timeout) { };

            template <class Handle>
            void operator() (Handle & handle) const {
                handle.replace_raw(
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
            }

            private:

            std::string key;
            std::string value;
            boost::uint16_t flags;
            time_t timeout;
            time_t failover_timeout;
        };
    } // namespace detail

    template <typename T>
    inline detail::raw_replace_directive<> raw_replace(T _key, std::string const & value, time_t timeout=0, boost::uint16_t flags = 0) {
        return detail::raw_replace_directive<>(std::string(_key), std::string(value), flags, timeout, timeout);
    }

} // namespace memcache

#endif


