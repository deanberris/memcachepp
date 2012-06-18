
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

#ifndef MEMCACHEPP_PARSER_HPP__
#define MEMCACHEPP_PARSER_HPP__

#include <list>

#include <boost/cstdint.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/tuple.hpp>

BOOST_FUSION_DEFINE_STRUCT((memcache)(detail), parsed_command,
    (std::string, key_name)
    (boost::uint16_t, flags)
    (size_t, size)
    (boost::uint64_t, cas)
    (std::string, data));

namespace memcache { namespace detail {

    namespace fusion = boost::fusion;
    
    struct value_response_grammar : boost::spirit::qi::grammar<std::string::const_iterator, 
        std::list<parsed_command>()>
    {
        value_response_grammar() : base_type(start), data_size(0) {

            using boost::spirit::qi::alnum;
            using boost::spirit::qi::punct;
            using boost::spirit::qi::uint_;
            using boost::spirit::qi::ulong_long;
            using boost::spirit::qi::lit;
            using boost::spirit::qi::char_;
            using boost::spirit::qi::attr;
            using boost::spirit::qi::repeat;

            start %= 
                *(
                    lit("VALUE ")
                    >> +(alnum|punct) >> ' '
                    >> uint_ >> ' '
                    >> uint_[boost::phoenix::ref(data_size) = boost::spirit::qi::_1]
                    >> ((' ' >> ulong_long) | attr(0)) // get the cas if any, 0 otherwise
                    >> lit("\r\n") >> repeat(boost::phoenix::ref(data_size))[char_] >> lit("\r\n")
                )
                >> lit("END\r\n");
        }

        boost::spirit::qi::rule<std::string::const_iterator, std::list<parsed_command>()> start;

        // qi::locals causes a problem with Phoenix v3 + decltype for result of
        size_t data_size;
    };


    template <class callback_container_type>
    struct execute_callbacks {
        explicit execute_callbacks(callback_container_type & callbacks_, boost::uint64_t & cas_value_)
        : callbacks(callbacks_), cas_value(cas_value_) {}
        execute_callbacks(execute_callbacks const & other)
        : callbacks(other.callbacks), cas_value(other.cas_value) {}
        void operator()(parsed_command const & result) const {
            if (result.key_name.empty()) return;
            callbacks[result.key_name](result.data);
            cas_value = result.cas;
        }
        callback_container_type & callbacks;
        boost::uint64_t & cas_value;
    };

    template <class callback_container_type>
    bool parse_response(std::string const & buffer, callback_container_type & callbacks, boost::uint64_t & cas_value) {
        // FIXME pass out the flags too?

        value_response_grammar value_response;

        std::list<parsed_command> result;
        std::string::const_iterator begin = buffer.begin()
                                    , end = buffer.end();
        if (!boost::spirit::qi::parse(
            begin, end              // the input range
            , value_response        // the context
            , result                // the synthesized attribute
            ))
        {
            return false;
        }

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

