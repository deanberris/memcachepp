
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEMCACHE_POLICIES_TEXT_INTERCHANGE_HPP__
#define MEMCACHE_POLICIES_TEXT_INTERCHANGE_HPP__

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>

namespace memcache { namespace policies {

        template <class tag = tags::default_tag>
        struct text_interchange {
            //text_interchange() { };
            typedef boost::archive::text_oarchive oarchive;
            typedef boost::archive::text_iarchive iarchive;
            typedef boost::archive::archive_exception archive_exception;
            protected: ~text_interchange() { };
        };
        
} // namespace policies

} // namespace memcache

#endif // MEMCACHE_POLICIES_TEXT_INTERCHANGE_HPP__

