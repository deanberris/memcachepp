
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 * The main Memcache++ include file. This pulls in all the required
 * libraries and Memcache++ headers.
 *
 * @author Dean Michael Berris <dmberris@friendster.com>
 */

#ifndef MEMCACHEPP_HPP_20080123
#define MEMCACHEPP_HPP_20080123

#ifndef MEMCACHE_TIMEOUT
#define MEMCACHE_TIMEOUT 500
#endif

#include <map>
#include <numeric>
#include <istream>
#include <sstream>

#include <memcachepp/memcache/detail/directives.hpp>
#include <memcachepp/memcache/detail/read_handler.hpp>
#include <memcachepp/memcache/detail/deserializer.hpp>
#include <memcachepp/memcache/exceptions.hpp>
#include <memcachepp/memcache/tags.hpp>
#include <memcachepp/memcache/server_pool.hpp>
#include <memcachepp/memcache/parser.hpp>
#include <memcachepp/memcache/policies.hpp>
#include <memcachepp/memcache/handle_concept.hpp>
#include <memcachepp/memcache/handle.hpp>
#include <memcachepp/memcache/request.hpp>

// Include the fluent interface
#include <memcachepp/memcache/fluent.hpp>

#include <boost/fusion/tuple.hpp>

namespace memcache {
    
#ifndef _REENTRANT
    typedef basic_handle<> handle ;
    typedef basic_request<> request ;
#else
    typedef basic_handle<policies::boost_threading<> > handle ;
    typedef basic_request<policies::boost_threading<> > request ;
#endif

    BOOST_CONCEPT_ASSERT((Handle<handle>));

}

#endif

