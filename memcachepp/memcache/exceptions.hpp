
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_EXCEPTIONS_HPP__
#define __MEMCACHE_DETAIL_EXCEPTIONS_HPP__

#include <exception>
#include <string>

#include <memcachepp/memcache/tags.hpp>

namespace memcache { 

    using std::runtime_error;
    using std::string;

    struct exception : runtime_error {
        exception(string const & str_)
            : runtime_error(str_)
        {}
        virtual ~exception() throw () {}
    };

    struct key_not_found : exception {
        explicit key_not_found (string const & key) :
            exception((string("Key not found: ") + key).c_str()) { };

        ~key_not_found() throw () { }
    };

    struct malformed_data : exception {
        explicit malformed_data (string const & data) :
            exception((string("Malformed data: ") + data).c_str()) { };
            
        ~malformed_data() throw () { }
    };

    struct invalid_key : exception {
        explicit invalid_key (string const & key) :
            exception((string("Invalid key: ") + key).c_str()) { };

        ~invalid_key() throw () { }
    };

    struct no_available_servers : exception {
        no_available_servers() :
            exception("No available servers.") { };
        ~no_available_servers() throw () { }
    };
    
    struct offset_out_of_bounds : exception {
        explicit offset_out_of_bounds (size_t offset) :
            exception((string("Offset out of bounds: ") + 
                boost::lexical_cast<string>(offset)).c_str()) { };

        ~offset_out_of_bounds() throw () { }
    };

    struct key_not_stored : exception {
        explicit key_not_stored (string const & key) :
            exception((string("Key not stored: ") + key).c_str()) { };

        ~key_not_stored() throw () { }
    };

    struct invalid_response_found : exception {
        explicit invalid_response_found (string const & key) :
            exception((string("Invalid response found for key: ") + key).c_str()) { };

        ~invalid_response_found() throw () { }
    };

} // namespace memcache

#endif // __MEMCACHE_DETAIL_EXCEPTIONS_HPP__
