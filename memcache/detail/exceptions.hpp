
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_EXCEPTIONS_HPP__
#define __MEMCACHE_DETAIL_EXCEPTIONS_HPP__

#include <exception>
#include <string>

#include "tags.hpp"

namespace memcache { namespace detail {

    template <typename tag>
    struct key_not_found_impl : std::runtime_error {
        explicit key_not_found_impl (std::string const & key) :
            std::runtime_error((std::string("Key not found: ") + key).c_str()) { };

        ~key_not_found_impl() throw () { };
    };

    template <typename tag>
    struct malformed_data_impl : std::runtime_error {
        explicit malformed_data_impl (std::string const & data) :
            std::runtime_error((std::string("Malformed data: ") + data).c_str()) { };
            
        ~malformed_data_impl() throw () { };
    };

    template <typename tag>
    struct invalid_key_impl : std::runtime_error {
        explicit invalid_key_impl (std::string const & key) :
            std::runtime_error((std::string("Invalid key: ") + key).c_str()) { };

        ~invalid_key_impl() throw () { };
    };

    template <typename tag>
    struct no_available_servers_impl : std::runtime_error {
        no_available_servers_impl() :
            std::runtime_error("No available servers.") { };
        ~no_available_servers_impl() throw () { };
    };
    
    template <typename tag>
    struct offset_out_of_bounds_impl : std::runtime_error {
        explicit offset_out_of_bounds_impl (size_t offset) :
            std::runtime_error((std::string("Offset out of bounds: ") + 
                boost::lexical_cast<std::string>(offset)).c_str()) { };

        ~offset_out_of_bounds_impl() throw () { };
    };

    template <typename tag>
    struct key_not_stored_impl : std::runtime_error {
        explicit key_not_stored_impl (std::string const & key) :
            std::runtime_error((std::string("Key not stored: ") + key).c_str()) { };

        ~key_not_stored_impl() throw () { };
    };

    template <typename tag>
    struct invalid_response_found_impl : std::runtime_error {
        explicit invalid_response_found_impl (std::string const & key) :
            std::runtime_error((std::string("Invalid response found for key: ") + key).c_str()) { };

        ~invalid_response_found_impl() throw () { };
    };

}; // namespace detail

}; // namespace memcache

#endif // __MEMCACHE_DETAIL_EXCEPTIONS_HPP__
