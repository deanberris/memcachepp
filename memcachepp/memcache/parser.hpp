
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

#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/qi_parse_attr.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_uint.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_domain.hpp>
#include <boost/spirit/include/support_argument.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/cstdint.hpp>
#include <list>

namespace memcache { namespace detail {

    using namespace boost::spirit::qi;
    using namespace boost::phoenix;
    namespace fusion = boost::fusion;
    
    struct value_response_grammar : 
        grammar<std::string::const_iterator, 
            std::list<
                fusion::vector<
                    std::string // the key name
                    , boost::uint16_t // the flags
                    , size_t // the size
                    , boost::optional<boost::uint64_t> // the cas value
                    , std::string // the data
                >
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
            ;

            cas %=
                ulong_long
            ;

            size %=
                uint_
            ;

            start %= 
                *(
                    lit("VALUE ")
                    >> key 
                    >> ' '
                    >> flags
                    >> ' '
                    >> size[_a = boost::spirit::qi::labels::_1]
                    >> -(lit(' ') >> cas)
                    >> lit("\r\n")
                    >> data(_a)
                    >> lit("\r\n")
                )
                >> lit("END\r\n")
            ;
        }

        rule<std::string::const_iterator, std::string()> key;
        rule<std::string::const_iterator, boost::uint16_t()> flags;
        rule<std::string::const_iterator, size_t()> size;
        rule<std::string::const_iterator, std::string(size_t)> data;
        rule<std::string::const_iterator, boost::uint64_t()> cas;
        rule<std::string::const_iterator, 
            std::list<
                fusion::vector<
                    std::string // the key name
                    , boost::uint16_t // the flags
                    , size_t // the size of the data
                    , boost::optional<boost::uint64_t> // the cas value
                    , std::string // the data
                >
            >(),
            locals<
                size_t // the size of the data
            >
            > start;
    };

    template <class callback_container_type>
    struct execute_callbacks {
        explicit execute_callbacks(callback_container_type & callbacks_, boost::uint64_t & cas_value_)
        : callbacks(callbacks_), cas_value(cas_value_) {}
        execute_callbacks(execute_callbacks const & other)
        : callbacks(other.callbacks), cas_value(other.cas_value) {}
        template <class Tuple>
        void operator()(Tuple const & result) const {
            if (fusion::get<0>(result) == "") return;

            callbacks[fusion::get<0>(result)](fusion::get<4>(result));
            if (fusion::get<3>(result)) {
                cas_value = *fusion::get<3>(result);
            }
        }
        callback_container_type & callbacks;
        boost::uint64_t & cas_value;
    };

    template <class callback_container_type>
    bool parse_response(std::string const & buffer, callback_container_type & callbacks, boost::uint64_t & cas_value) {
        // FIXME pass out the flags too?
        static value_response_grammar value_response;
        std::list<
            fusion::vector<
                std::string
                , boost::uint16_t
                , size_t
                , boost::optional<boost::uint64_t>
                , std::string
            >
        > result;
        std::string::const_iterator begin = buffer.begin()
                                    , end = buffer.end();
        parse(
            begin, end              // the input range
            , value_response        // the context
            , result                // the synthesized attribute
            );

        if (boost::empty(result))
            return false;

        std::for_each(
            result.begin(),
            result.end(),
            execute_callbacks<callback_container_type>(callbacks, cas_value)
            );
        
        return true;
    }
    
} // namespace detail

} // namespace memcache

#endif

