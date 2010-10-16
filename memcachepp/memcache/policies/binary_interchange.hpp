
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_POLICIES_BINARY_INTERCHANGE_HPP__
#define __MEMCACHE_POLICIES_BINARY_INTERCHANGE_HPP__

#include <memcachepp/memcache/policies/tags.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>

namespace memcache { namespace policies {

    template <class tag = tags::default_tag>
    struct binary_interchange {
        //binary_interchage() { };
        typedef boost::archive::binary_oarchive oarchive;
        typedef boost::archive::binary_iarchive iarchive;
        typedef boost::archive::archive_exception archive_exception;
        protected: ~binary_interchange() { };
    };
        
} // namespace policies

} // namespace memcache

#endif // __MEMCACHE_POLICIES_BINARY_INTERCHANGE_HPP__
