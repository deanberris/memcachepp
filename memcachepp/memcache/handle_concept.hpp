#ifndef MEMCACHEPP_MEMCACHE_HANDLE_CONCEPT_HPP_
#define MEMCACHEPP_MEMCACHE_HANDLE_CONCEPT_HPP_

// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/concept_check.hpp>

namespace memcache {

    template <class H>
        struct Handle : boost::DefaultConstructible<H> {
            typedef typename H::server_info server_info;
            typedef typename H::pool_info pool_info;

            BOOST_CONCEPT_USAGE(Handle) {
                server_info s = {} ; // server_info must be a POD type
                pool_info p = {} ; // pool_info must be a POD type

                H h; // handle is default constructible
                h.add_server("server", s);
                h.add_pool("pool", p);
                h.connect();
                size_t offset = 0;
                time_t delay = 0;
                h.delete_(offset, "key", delay);
                int d;
                h.get<int>(offset, "key", d);
                std::string c;
                h.get_raw(offset, "key", c);
                h.is_connected("server");
                size_t pool_count = h.pool_count();
                pool_count = 0;
                size_t server_count = h.server_count();
                server_count = 0;
                time_t expiration = 0;
                time_t failover_expiration = 0;
                uint16_t flag = 0;
                h.set<int>(offset, "key", d, expiration, failover_expiration, flag);
                std::string raw = "";
                h.set_raw(offset, "kye", raw, expiration, failover_expiration, flag);
            }
        };

} // namespace memcache

#endif

