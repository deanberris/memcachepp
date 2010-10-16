
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_POLICIES_STRING_PRESERVE__
#define __MEMCACHE_POLICIES_STRING_PRESERVE__

#include <memcachepp/memcache/policies/tags.hpp>
#include <boost/ref.hpp>
#include <sstream>
#include <string>

namespace memcache { namespace policies {

    namespace detail {

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

        inline oarchive & operator<< ( oarchive & archive, std::string const & str) {
            archive._os.str(str);
            return archive;
        }

        inline iarchive & operator>> ( iarchive & archive, std::string & str) {
            std::stringbuf buffer;
            while (!archive._is.eof()) {
                archive._is.get(buffer);
                if (archive._is.peek() == '\n')
                    buffer.sputc(archive._is.get());
            };
            str = buffer.str();
            return archive;
        }

    } // namespace detail

    template <class tag = tags::default_tag>
        struct string_preserve_impl {
            typedef detail::oarchive oarchive;
            typedef detail::iarchive iarchive;
            typedef std::runtime_error archive_exception;

            protected: ~string_preserve_impl() { };
        };

    typedef string_preserve_impl<> string_preserve;

} // namespace policies

} // namespace memcache

#endif // __MEMCACHE_POLICIES_STRING_PRESERVE__

