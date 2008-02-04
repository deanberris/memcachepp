
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_RAW_GET_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_RAW_GET_HPP__

#include "../tags.hpp"

namespace memcache { namespace detail {

    template < class tag = tags::default_tag >
    struct raw_get_directive {
        explicit raw_get_directive(std::string const & key, std::string & value) :
            _key(key), _value(value) { };
        
        template <typename _T>
        void operator() (_T & handle) const {
            handle.get_raw(
                    handle.hash(
                        _key, 
                        handle.pool_count()
                        ), 
                    _key, 
                    _value
                    );
        };

        private:

        mutable std::string _key;
        mutable std::string & _value;
    };

};

};

#endif // __MEMCACHE_DETAIL_DIRECTIVES_RAW_GET_HPP__

