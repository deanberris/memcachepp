
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DESERIALIZER_HPP__
#define __MEMCACHE_DETAIL_DESERIALIZER_HPP__

#include <string>
#include <sstream>
#include <istream>

namespace memcache { namespace detail {

    template <typename T, class data_interchange_policy>
    struct deserializer {
        T & _holder;
        
        explicit deserializer(T & holder) :
            _holder(holder) { };
            
        void operator() (std::string const & data) const {
            std::stringbuf byte_string_buffer(data);
            std::istream byte_buffer_stream(&byte_string_buffer);
            typename data_interchange_policy::iarchive ar(byte_buffer_stream);
            ar >> _holder;
        };
    };

}; // namespace detail

}; // namespace memcache

#endif // __MEMCACHE_DETAIL_DESERIALIZER_HPP__

