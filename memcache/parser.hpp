
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 * The protocol parser implementation. This uses Boost.Spirit
 * (http://spirit.sourceforge.net/) to define the parser for
 * the protocol response of the memcache server.
 *
 * @author Dean Michael Berris <dmberris@friendster.com>
 */

#ifndef __MEMCACHEPP_PARSER_HPP__
#define __MEMCACHEPP_PARSER_HPP__

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/spirit/utility/loops.hpp>
#include <boost/spirit/phoenix/statements.hpp>

namespace memcache { namespace detail {

    using namespace boost::spirit;
    using namespace phoenix;
    
    template <class callback_container>
    struct protocol_parser {
        callback_container & _callbacks;
        rule<scanner<std::string::const_iterator> > response, response_header;

        std::string key, data;
        size_t size;
        
        struct perform_callback {
            perform_callback(callback_container & callbacks,
                std::string const & key,
                std::string const & data) : 
                    _callbacks(callbacks),
                    _key(key),
                    _data(data)
                    { }
            mutable callback_container & _callbacks;
            std::string const & _key;
            std::string const & _data;
            
            template <typename T>
            void operator() (T, T) const {
                _callbacks[_key](_data);
            };
        };
        
        explicit protocol_parser(callback_container & callbacks)
            : _callbacks(callbacks) 
        { 

            response_header = chseq_p("VALUE ")
                                >> (+(alnum_p|punct_p))[var(key) = construct_<std::string>(arg1, arg2)] // key
                                >> ch_p(' ')
                                >> uint_p // flags
                                >> ch_p(' ')
                                >> uint_p[var(size) = arg1] // bytes
                            ;
            response = +(response_header 
                            >> chseq_p("\r\n") 
                            >> lexeme_d[repeat_p(boost::cref(size))[anychar_p]]
                                [
                                    if_(arg1 != arg2) [
                                        var(data) = construct_<std::string>(arg1, arg2)
                                    ].else_[
                                        var(data) = ""
                                    ]
                                ] // data
                            >> chseq_p("\r\n")
                            >> eps_p[perform_callback(boost::ref(_callbacks),
                                boost::cref(key), boost::cref(data))] // callback
                            )
                        >> chseq_p("END\r\n")
                        >> end_p
                        ;
        };

    };

    template <class callback_container_type>
    bool parse_response(std::string const & buffer, callback_container_type & callbacks) {
        protocol_parser<callback_container_type> parser_instance(callbacks);
        return parse(buffer.begin(), buffer.end(), parser_instance.response).full;
    };
    
}; // namespace detail

}; // namespace memcache

#endif

