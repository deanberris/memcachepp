
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEMCACHE_POLICIES_DEFAULT_HASH_HPP__
#define MEMCACHE_POLICIES_DEFAULT_HASH_HPP__

#include <boost/crc.hpp>

namespace memcache { namespace policies {

    template <class tag = tags::default_tag>
    struct default_hash {
        size_t hash(std::string const & key, size_t server_count) const {
            assert(server_count != 0);
            boost::crc_32_type crc32_computer;
            crc32_computer.process_bytes(key.c_str(), key.size());
            return crc32_computer.checksum() % server_count;
        };
        protected: ~default_hash() { };
    };

} // namespace policies

} // namespace memcache

#endif // MEMCACHE_POLICIES_DEFAULT_HASH_HPP__

