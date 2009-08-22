
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_POLICIES_BOOST_THREADING_HPP__
#define __MEMCACHE_POLICIES_BOOST_THREADING_HPP__

#include <memcachepp/memcache/policies/tags.hpp>

#ifdef _REENTRANT
#include <boost/thread/mutex.hpp>
#endif

namespace memcache { namespace policies {

#ifdef _REENTRANT

    template <class tag = tags::default_tag>
    struct boost_threading {
        boost_threading () :
            _mutex() { };

        struct lock : boost::mutex::scoped_lock {
            explicit lock(boost_threading & object) :
                boost::mutex::scoped_lock(object._mutex) { };
            
            ~lock() { };
        };

        friend struct lock;
        
        protected: 
        
        ~boost_threading() { };

        boost::mutex _mutex;
    };

#endif    

}; // namespace policies

}; // namespace memcache

#endif // __MEMCACHE_POLICIES_BOOST_THREADING_HPP__

