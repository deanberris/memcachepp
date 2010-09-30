
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007-2010 (c) Dean Michael Berris <mikhailberis@gmail.com>
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

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/cstdint.hpp>

namespace memcache { namespace detail {

    using namespace boost::spirit::qi;
    using namespace boost::phoenix;
    namespace fusion = boost::fusion;
    
    struct value_response_grammar : 
        grammar<std::string::const_iterator, 
            fusion::vector<
                std::string // the key name
                , boost::uint16_t // the flags
                , size_t // the size
                , boost::optional<boost::uint64_t> // the cas value
                , std::string // the data
            >(),
            locals<size_t>
        >
    {
        value_response_grammar() : base_type(start) {
            using namespace boost::spirit::qi::labels;
            using namespace boost::phoenix::arg_names;
            key %= 
                +(alnum|punct)
            ;

            flags %=
                uint_
            ;

            data %=
                repeat(_r1)[char_]
                >> lit("\r\nEND\r\n")
            ;

            cas %=
                ulong_long
            ;

            size %=
                ulong_
            ;

            start %= 
                lit("VALUE ")
                >> key 
                >> ' '
                >> flags
                >> ' '
                >> size[_a = boost::spirit::qi::_1]
                >> -(lit(' ') >> cas)
                >> lit("\r\n")
                >> data(_a)
                >> lit("\r\nEND\r\n")
            ;
        }

        rule<std::string::const_iterator, std::string()> key;
        rule<std::string::const_iterator, boost::uint16_t()> flags;
        rule<std::string::const_iterator, size_t()> size;
        rule<std::string::const_iterator, std::string(size_t)> data;
        rule<std::string::const_iterator, boost::uint64_t()> cas;
        rule<std::string::const_iterator, 
            fusion::vector<
                std::string // the key name
                , boost::uint16_t // the flags
                , size_t // the size of the data
                , boost::optional<boost::uint64_t> // the cas value
                , std::string // the data
            >(),
            locals<
                size_t // the size of the data
            >
            > start;
    };

    template <class callback_container_type>
    bool parse_response(std::string const & buffer, callback_container_type & callbacks, boost::uint64_t & cas_value) {
        // FIXME pass out the flags too?
        static value_response_grammar value_response;
        fusion::vector<
            std::string
            , boost::uint16_t
            , size_t
            , boost::optional<boost::uint64_t>
            , std::string
        > result;
        std::string::const_iterator begin = buffer.begin()
                                    , end = buffer.end();
        bool ok = parse(
            begin, end              // the input range
            , value_response        // the context
            , result                // the synthesized attribute
            );
        callbacks[fusion::get<0>(result)](fusion::get<4>(result));
        if (fusion::get<3>(result)) {
            cas_value = *fusion::get<3>(result);
        }
        return ok;
    };
    
}; // namespace detail

}; // namespace memcache

#endif

