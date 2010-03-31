#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <memcachepp/memcache/parser.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <map>

typedef std::map<std::string, boost::function<void(std::string const &)> > callback_container_type;

void printer (std::string const & str) {
    // no op
};

BOOST_AUTO_TEST_CASE ( value_response_parsing ) {
    using namespace boost::spirit;
    callback_container_type callbacks;
    callbacks["key"] = printer;
    callbacks["key_1"] = printer;
    char response_[] = "VALUE key 0 4\r\na\0a\0\r\nEND\r\n";
    std::string response(response_, 26);
    boost::uint64_t cas_value = 1u;
    BOOST_CHECK (memcache::detail::parse_response(response, callbacks, cas_value));
    char response_1[] = "VALUE key 0 4\r\na\0a\0\r\nVALUE key_1 0 4\r\na\0a\0\r\nEND\r\n";
    std::string response1(response_1, 49);
    BOOST_CHECK (memcache::detail::parse_response(response1, callbacks, cas_value));
    char response_2[] = "VALUE key 0 4 0\r\na\0a\0\r\nEND\r\n";
    std::string response2(response_2, 28);
    BOOST_CHECK (memcache::detail::parse_response(response2, callbacks, cas_value));
    BOOST_CHECK_EQUAL ( 0u, cas_value );
};

BOOST_AUTO_TEST_CASE ( empty_response_parsing ) {
    using namespace boost::spirit;
    callback_container_type callbacks;
    callbacks["key2"] = printer;
    std::string response("END\r\n");
    boost::uint64_t cas_value = 0u;
    BOOST_CHECK (!memcache::detail::parse_response(response, callbacks, cas_value));
};

