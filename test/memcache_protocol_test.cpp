
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
  * Memcache Protocol Test implementation.
  *
  * @author Dean Michael Berris <dmberris@friendster.com>
  *
  */

#define BOOST_TEST_MODULE Memcache Protocol Test
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <memcache++/memcache.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <boost/serialization/vector.hpp>

BOOST_AUTO_TEST_CASE ( initializer_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211) << memcache::connect ;
    // Dean Michael Berris (Oct. 23, 2007)
    // deprecated: servers is now private
    // BOOST_CHECK ( mc.servers.find("localhost:11211") != mc.servers.end() );
    BOOST_CHECK ( mc.is_connected("localhost:11211") == true );
    mc << memcache::server("localhost", 11299) << memcache::connect ;

    // Dean Michael Berris (Oct. 23, 2007)
    // deprecated: servers is now private
    // BOOST_CHECK ( mc.servers.find("localhost:11299") != mc.servers.end() );
    BOOST_CHECK ( mc.is_connected("localhost:11299") == false );
};

BOOST_AUTO_TEST_CASE ( set_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211) 
        << memcache::server("localhost", 11212)
        << memcache::connect ;

    int some_key = 0;
    try { mc << memcache::delete_("blah+blah,blah?"); } catch (...) { }
    
    BOOST_CHECK_THROW ( mc << memcache::get("blah+blah,blah?",some_key); , memcache::key_not_found );
    BOOST_CHECK_NO_THROW ( mc << memcache::set("blah+blah,blah?", 100); );
    BOOST_CHECK_NO_THROW ( mc << memcache::get("blah+blah,blah?", some_key); );
    BOOST_CHECK_EQUAL ( 100, some_key );
};

BOOST_AUTO_TEST_CASE ( failover_expiration_test ) {
    memcache::handle mc;
    mc << memcache::server("aodjfgbie", 11111)
        << memcache::server("localhost", 11211)
        << memcache::connect ;
    
    BOOST_CHECK_NO_THROW ( mc << memcache::set("4", 100, memcache::expire(1), memcache::failover_expire(1)) ; );
    sleep(2);
    int some_value;
    BOOST_CHECK_THROW ( mc << memcache::get("4", some_value) ; , memcache::key_not_found );

    BOOST_CHECK_NO_THROW ( mc << memcache::set("4", 100, memcache::failover_expire(1)) ; );
    sleep(2);
    BOOST_CHECK_THROW ( mc << memcache::get("4", some_value) ; , memcache::key_not_found );

    BOOST_CHECK_NO_THROW ( mc << memcache::set("4", 100, memcache::failover_expire(1), memcache::expire(100)) ; );
    sleep(2);
    BOOST_CHECK_THROW ( mc << memcache::get("4", some_value) ; , memcache::key_not_found );
};

BOOST_AUTO_TEST_CASE ( corrupted_data_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211)
        << memcache::connect ;

    {
        using boost::asio::ip::tcp ;
        tcp::iostream mc_connection("localhost", "11211", tcp::resolver::query::numeric_service);
        mc_connection << "set corrupted_data 0 0 1\r\n0\r\n" << std::flush;
        std::string response;
        mc_connection >> response;
        if (response != "STORED")
            std::cerr << "ERROR: " << response << '\n';
    }
    
    std::vector<int> test_vector;
    BOOST_CHECK_THROW ( mc << memcache::get("corrupted_data",test_vector); , memcache::malformed_data );
};

void do_get(memcache::handle & mc, std::string const & key, int & value) {
    mc << memcache::get(key, value);
};

BOOST_AUTO_TEST_CASE ( threaded_set_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211)
        << memcache::server("localhost", 11212)
        << memcache::connect;

    try { mc << memcache::delete_("key1"); } catch (...) { };
    try { mc << memcache::delete_("key2"); } catch (...) { };

    mc << memcache::set("key1", 1024) << memcache::set("key2", 1215);
    
    int some_key = 0, some_other_key = 0;
    boost::thread t1(boost::bind(do_get, boost::ref(mc), std::string("key1"), 
        boost::ref(some_key)));
    boost::thread t2(boost::bind(do_get, boost::ref(mc), std::string("key2"),
        boost::ref(some_other_key)));
    
    t2.join();
    t1.join();

    BOOST_CHECK_EQUAL ( some_key, 1024 );
    BOOST_CHECK_EQUAL ( some_other_key, 1215 );
};

BOOST_AUTO_TEST_CASE ( key_validation_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211)
        << memcache::server("localhost", 11212)
        << memcache::connect;
    
    BOOST_CHECK_THROW ( mc << memcache::delete_("key1\r");, memcache::invalid_key );
    BOOST_CHECK_THROW ( mc << memcache::delete_("key2 ");, memcache::invalid_key );
    BOOST_CHECK_THROW ( mc << memcache::delete_("key1\t");, memcache::invalid_key );
};

BOOST_AUTO_TEST_CASE ( grouped_requests_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211)
        << memcache::server("localhost", 11212)
        << memcache::connect;
    int integer = 0;
    double a_double = 0.0, expected = 10.2;
    std::string a_string;

    try { mc << memcache::delete_("integer") ; } catch (...) { };
    try { mc << memcache::delete_("double") ; } catch (...) { };
    try { mc << memcache::delete_("some_string") ; } catch (...) { };

    mc << memcache::set("integer", 1024) << memcache::set("double", expected)
        << memcache::set("some_string", std::string("Some String!")) ;

    memcache::request mc_request(mc);
    mc_request << memcache::get("integer", integer)
        << memcache::get("double", a_double)
        << memcache::get("some_string", a_string);
    mc_request << memcache::commit;

    BOOST_CHECK_EQUAL (integer, 1024);
    BOOST_CHECK_EQUAL (a_double, expected);
    BOOST_CHECK_EQUAL (a_string, std::string("Some String!"));
};

BOOST_AUTO_TEST_CASE ( raw_set_get_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211) << memcache::connect;
    std::string value("");

    try { mc << memcache::delete_("key") ; } catch (...) { };

    BOOST_CHECK_NO_THROW (mc << memcache::raw_set("key", std::string("value"))) ;
    BOOST_CHECK_NO_THROW (mc << memcache::raw_get("key", value)) ;
    BOOST_CHECK_EQUAL (value, "value");
};

BOOST_AUTO_TEST_CASE ( raw_set_get_with_null_test ) {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211) << memcache::connect;

    static char test_data[] = "this\0has\nspecial\tcharacters\r\nembedded";
    std::string test(test_data, test_data + sizeof(test_data));
    std::string value;

    try { mc << memcache::delete_("key") ; } catch (...) { };

    BOOST_CHECK_NO_THROW (mc << memcache::raw_set("key", test)) ;
    BOOST_CHECK_NO_THROW (mc << memcache::raw_get("key", value)) ;
    BOOST_CHECK_EQUAL (value, test);
};

void do_call() {
    memcache::handle mc;
    mc << memcache::server("localhost", 11211) << memcache::connect;
    
    std::string value("");
    try { mc << memcache::set("multi_thread_key", std::string("1234092heg02i3nr")) ; } catch (...) { };
    try { mc << memcache::get("multi_thread_key", value) ; } catch (...) { };
};

BOOST_AUTO_TEST_CASE ( multi_thread_multi_handle ) {
    boost::thread_group threads;
    for (unsigned int i = 0; i < 10; ++i)
        threads.create_thread(do_call);
    threads.join_all();
};

