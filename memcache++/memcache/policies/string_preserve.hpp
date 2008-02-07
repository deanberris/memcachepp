
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_POLICIES_STRING_PRESERVE__
#define __MEMCACHE_POLICIES_STRING_PRESERVE__

#include "tags.hpp"
#include <boost/ref.hpp>
#include <sstream>
#include <string>

namespace memcache { namespace policies {

    template <class tag = tags::default_tag>
        struct string_preserve_impl {
            struct oarchive {
                explicit oarchive(std::ostringstream & os)
                    : _os(os)
                    { };
                std::ostringstream & _os;
            };
            
            struct iarchive {
                explicit iarchive(std::istream & is)
                    : _is(is)
                    { };
                std::istream & _is;
            };

            typedef std::runtime_error archive_exception;

            protected: ~string_preserve_impl() { };
        };

    typedef string_preserve_impl<> string_preserve;

}; // namespace policies

    inline  policies::string_preserve::oarchive & operator<< ( policies::string_preserve::oarchive & archive, std::string const & str) {
        archive._os << str;
        return archive;
    };

    inline  policies::string_preserve::iarchive & operator>> ( policies::string_preserve::iarchive & archive, std::string & str) {
        archive._is >> str;
        return archive;
    };

}; // namespace memcache

#endif // __MEMCACHE_POLICIES_STRING_PRESERVE__

