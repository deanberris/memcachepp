
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 * The main Memcache++ include file. This pulls in all the required
 * libraries and Memcache++ headers.
 *
 * @author Dean Michael Berris <dmberris@friendster.com>
 */

#ifndef MEMCACHEPP_HPP_20080123
#define MEMCACHEPP_HPP_20080123

#ifndef MEMCACHE_TIMEOUT
#define MEMCACHE_TIMEOUT 500
#endif

#include <map>
#include <set>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/cast.hpp>
#include <boost/foreach.hpp>
#include <numeric>
#include <istream>
#include <sstream>

#include "memcache/policies.hpp"
#include "memcache/detail/directives.hpp"
#include "memcache/parser.hpp"
#include "memcache/detail/deserializer.hpp"
#include "memcache/detail/exceptions.hpp"
#include "memcache/tags.hpp"
#include "memcache/detail/read_handler.hpp"
#include "memcache/server_pool.hpp"

#include <boost/fusion/tuple.hpp>

namespace memcache {
    
    typedef detail::key_not_found_impl<tags::exception> 
        key_not_found ;
    
    typedef detail::key_not_stored_impl<tags::exception> 
        key_not_stored ;
    
    typedef detail::offset_out_of_bounds_impl<tags::exception> 
        offset_out_of_bounds ;

    typedef detail::invalid_key_impl<tags::exception> 
        invalid_key ;

    typedef detail::no_available_servers_impl<tags::exception> 
        no_available_servers ;

    typedef detail::invalid_response_found_impl<tags::exception> 
        invalid_response_found ;

    typedef detail::malformed_data_impl<tags::exception> 
        malformed_data ;
    
    typedef boost::function<void(std::string const &)> 
        callback_type;
    
    typedef std::map<std::string, callback_type> 
        callback_map;

    template <
        class threading_policy,
        class data_interchange_policy,
        class hash_policy
    >
    struct basic_request ; // forward declaration

    namespace fusion = boost::fusion ;
    
    template <
        class threading_policy = policies::default_threading<>, 
        class data_interchange_policy = policies::binary_interchange<>,
        class hash_policy = policies::default_hash<>
    >
    struct basic_handle : 
        threading_policy, 
        data_interchange_policy, 
        hash_policy {

        typedef boost::shared_ptr<boost::asio::ip::tcp::socket> connection_ptr;
        
        struct server_info {
            bool connected;
            std::string host, port;
            boost::system::error_code error;
            connection_ptr connection;
        };

        struct pool_info {
            enum { standalone = 0,
                replicating = 1,
                failover = 2 };
            typedef std::set<std::string> member_container;

            int status;
            member_container members;
        };

        typedef std::map<std::string, server_info> server_container;
        typedef std::map<std::string, pool_info> pool_container;
        
        basic_handle() : 
            threading_policy(), 
            data_interchange_policy(), 
            hash_policy() 
            { };
        
        void connect() { 
            typename threading_policy::lock scoped_lock(*this);
            for_each(servers.begin(), servers.end(), connect_impl(_service));
        };

        template <typename T> // T must be serializable
        void set(size_t offset, std::string const & key, T const & value, time_t expiration, time_t failover_expiration, uint16_t flags = 0) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            fusion::tie(connections, rehash) = get_connections(offset);
            size_t connections_size = connections.size();

            std::list<int> errors;

            std::transform(connections.begin(), connections.end(),
                    back_inserter(errors),
                    set_impl<T, data_interchange_policy>(
                        key, 
                        value, 
                        expiration, 
                        failover_expiration, 
                        flags, 
                        rehash
                        )
                    );

            if (connections_size ==
                    static_cast<size_t>(std::accumulate(errors.begin(), errors.end(), 0))) 
                throw key_not_stored(key);
        };

        void set_raw(size_t offset, std::string const & key, std::string const & value, time_t expiration, time_t failover_expiration, uint16_t flags = 0) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            fusion::tie(connections, rehash) = get_connections(offset);
            size_t connections_size = connections.size();

            std::list<int> errors;

            std::transform(connections.begin(), connections.end(),
                    back_inserter(errors),
                    set_impl<std::string, policies::string_preserve>(
                        key, 
                        value, 
                        expiration, 
                        failover_expiration, 
                        flags, 
                        rehash
                        )
                    );

            if (connections_size == 
                    static_cast<size_t>(std::accumulate(errors.begin(), errors.end(), 0)))
                throw key_not_stored(key);
        };

        void delete_(size_t offset, std::string const & key, time_t delay = 0) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            fusion::tie(connections, rehash) = get_connections(offset);
            size_t connections_size = connections.size();

            std::list<int> errors;
            transform(connections.begin(), connections.end(),
                    std::back_inserter(errors),
                    delete_impl(
                        key, 
                        delay
                        )
                    );

            if (connections_size ==
                    static_cast<size_t>(
                        std::accumulate(errors.begin(), errors.end(), 0)
                        )
               )
                throw key_not_found(key);
        };

        template <typename T> // T must be serializable
        void get(size_t offset, std::string const & key, T & holder) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            fusion::tie(connections, rehash) = get_connections(offset);

            typename connection_container::iterator connection_iterator = connections.begin();

            get_impl<T, data_interchange_policy>
                getter(key, holder);
            
            while (connection_iterator != connections.end()) {
                if (getter(*(connection_iterator++)))
                    return;
            };

            // when you get here, it means you didn't find the key
            throw key_not_found(key);
        };

        void get_raw(size_t offset, std::string const & key, std::string & holder) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            fusion::tie(connections, rehash) = get_connections(offset);

            typename connection_container::iterator connection_iterator = connections.begin();

            get_impl<std::string, policies::string_preserve>
                getter(key, holder);
            
            while (connection_iterator != connections.end()) {
                if (getter(*(connection_iterator++)))
                    return;
            };

            // when you get here, it means you didn't find the key
            throw key_not_found(key);
        };
        
        bool is_connected(std::string const & server_name) { 
            typename threading_policy::lock scoped_lock(*this);
            typename server_container::const_iterator iterator = servers.find(server_name);
            if (iterator == servers.end())
                return false;
            return iterator->second.connected;
        };
        
        ~basic_handle() { 
            typename threading_policy::lock scoped_lock(*this);
            pool_container().swap(pools);
            server_container().swap(servers);
            _service.stop();
        };

        friend class basic_request<threading_policy, data_interchange_policy, hash_policy>;

        typename server_container::size_type server_count() {
            return servers.size();
        };

        typename pool_container::size_type pool_count() {
            return pools.size();
        };

        void add_server(std::string const & server_name, server_info const & s_info) {
            servers.insert(
                    std::make_pair(server_name, s_info)
                    );
        };

        void add_pool(std::string const & pool_name, pool_info p_info) {
            pools.insert(
                    std::make_pair(pool_name, p_info)
                    );
        };

        private:
        
        server_container servers;
        pool_container pools;

        typedef std::vector<typename server_container::iterator> connection_container;

        inline fusion::tuple <connection_container, bool> get_connections(size_t offset) {
            typename pool_container::iterator pool_iterator;
            size_t counter = 0;
            for (pool_iterator = pools.begin();
                    pool_iterator != pools.end(), counter < offset;
                    ++counter, ++pool_iterator);

            bool past_end = false;
            bool rehash = false;
            connection_container connections;
            if (pool_iterator->second.status == pool_info::standalone) {
                typename server_container::iterator server_iterator
                    = servers.find(pool_iterator->first);
                while (!(server_iterator->second.connected)) {
                    rehash = true;
                    ++server_iterator;
                    if (server_iterator == servers.end()) {
                        if (past_end)
                            throw no_available_servers();
                        server_iterator = servers.begin();
                        past_end = true;
                    };
                };
                connections.push_back(server_iterator);
            } else {
                BOOST_FOREACH (std::string server_name, pool_iterator->second.members) {
                    typename server_container::iterator server_iterator
                        = servers.find(server_name);
                    if (server_iterator->second.connected)
                        connections.push_back(server_iterator);
                };
                
                while (connections.empty()) {
                    rehash = true;
                    ++pool_iterator;
                    if (pool_iterator == pools.end()) {
                        if (past_end)
                            throw no_available_servers();
                        pool_iterator = pools.begin();
                        past_end = true;
                    };
                    BOOST_FOREACH (std::string server_name, pool_iterator->second.members) {
                        typename server_container::iterator server_iterator
                            = servers.find(server_name);
                        if (server_iterator->second.connected)
                            connections.push_back(server_iterator);
                    };
                };
            };
            
            return fusion::make_tuple(connections, rehash);
        };

        boost::asio::io_service _service;

        template <typename T, typename set_interchange_policy>
            struct set_impl {
                std::string command;
                
                explicit set_impl(std::string const & key, T const & value, time_t expiration, time_t failover_expiration, uint16_t flags, bool rehash) 
                    { 
                        std::ostringstream output_bytes_stream;
                        typename set_interchange_policy::oarchive archive(output_bytes_stream);
                        archive << value;

                        std::ostringstream command_stream;
                        command_stream << "set " << key << " " << flags
                            << " " << (rehash ? failover_expiration : expiration)
                            << " " << output_bytes_stream.str().size() << "\r\n"
                            << output_bytes_stream.str() << "\r\n";
                        command = command_stream.str();
                    };
                
                template <typename value_type>
                    int operator() (value_type & server_iterator) {
                        boost::asio::streambuf buffer;
                        connection_ptr connection = server_iterator->second.connection;
                        try {
                            boost::asio::write(*connection,
                                boost::asio::buffer(const_cast<char*>(command.c_str()),
                                    sizeof(char) * command.size())
                                );
#ifdef _REENTRANT
                            detail::read_handler handler_instance(*connection, buffer,
                                boost::regex("\r\n"), MEMCACHE_TIMEOUT);
                            handler_instance();
#else
                            boost::asio::read_until(*connection,
                                buffer,
                                boost::regex("\r\n"));
#endif
                        } catch (boost::system::system_error & e) {
                            server_iterator->second.error = e.code();
                            server_iterator->second.connected = false;
                            server_iterator->second.connection.reset();
                            return 1; // indicate error
                        };

                        std::istream response(&buffer);
                        std::string line;
                        getline(response, line);

                        if (line == "STORED\r")
                            return 0; // indicate NO error

                        std::istringstream tokenizer(line);
                        std::string first_token;
                        tokenizer >> first_token;    
                        if ((first_token == "ERROR") || 
                            (first_token == "CLIENT_ERROR") ||
                            (first_token == "SERVER_ERROR"))
                            return 1; // indicate error

                        return 1; // indicate error
                    };
            };
        
        template <typename holder_type, class get_interchange_policy>
            struct get_impl {
                std::string _key;
                holder_type & _holder;
                std::string command;
                explicit get_impl(std::string const & key, holder_type & holder)
                    : _key(key), _holder(holder) 
                    {
                        std::ostringstream command_stream;
                        command_stream << "get " << key << "\r\n";
                        command = command_stream.str();
                    };
                
                template <typename T>
                    bool operator() (T & server_iterator) {
                        boost::asio::streambuf buffer;
                        connection_ptr connection = server_iterator->second.connection;
                        try {
                            boost::asio::write(*connection,
                                boost::asio::buffer(const_cast<char*>(command.c_str()),
                                    sizeof(char) * command.size())
                                );
#ifdef _REENTRANT
                            detail::read_handler handler_instance(*connection, buffer, 
                                boost::regex("(END\r\n)|(SERVER_ERROR\\S)|(CLIENT_ERROR\\S)|(ERROR\r\n)"),
                                MEMCACHE_TIMEOUT);
                            handler_instance();
#else
                            boost::asio::read_until(*connection,
                                buffer,
                            boost::regex("(END\r\n)|(SERVER_ERROR\\S)|(CLIENT_ERROR\\S)|(ERROR\r\n)"));
#endif
                        } catch (boost::system::system_error & e) {
                            server_iterator->second.error = e.code();
                            server_iterator->second.connected = false;
                            server_iterator->second.connection.reset();
                            return false; // wasn't able to get it
                        };

                        std::istream response(&buffer);
                        
                        callback_map callbacks;
                        callbacks[_key] = detail::deserializer<holder_type, get_interchange_policy>(_holder);
                        
                        std::ostringstream data;
                        std::string line;
                        while (getline(response, line)) {
                            if (response.eof()) break;
                            data << line << '\n';
                        };

                        std::string data_string(data.str());

                        try {
                            if (!detail::parse_response(data_string, callbacks)) {
                                std::istringstream tokenizer(data_string);
                                std::string first_token;
                                tokenizer >> first_token;
                                if (first_token == "END")
                                    return false; // wasn't able to get it
                                if ((first_token == "ERROR") || 
                                    (first_token == "CLIENT_ERROR") ||
                                    (first_token == "SERVER_ERROR"))
                                    throw invalid_response_found(data_string);
                            };
                        } catch (typename data_interchange_policy::archive_exception & e) {
                            std::ostringstream malformed_data_stream;
                            malformed_data_stream << command
                                << data_string;
                            throw malformed_data(malformed_data_stream.str());
                        };

                        return true;
                    };
                };
        
        struct delete_impl {
            std::string command;
            explicit delete_impl(std::string const & key, time_t delay)
                {
                    std::ostringstream command_stream;
                    command_stream << "delete " << key << ' ' << delay << "\r\n";
                    command = command_stream.str();
                };

            template <typename value_type>
                int operator() (value_type & server_iterator) {
                    boost::asio::streambuf buffer;
                    connection_ptr connection = server_iterator->second.connection;
                    try {
                        boost::asio::write(*connection,
                            boost::asio::buffer(const_cast<char*>(command.c_str()),
                                sizeof(char) * command.size())
                            );
#ifdef _REENTRANT
                            detail::read_handler handler_instance(*connection,
                                buffer, boost::regex("\r\n"), MEMCACHE_TIMEOUT);
                            handler_instance();
#else
                            boost::asio::read_until(*connection,
                                buffer, boost::regex("\r\n"));
#endif
                    } catch (boost::system::system_error & e) {
                        server_iterator->second.error = e.code();
                        server_iterator->second.connected = false;
                        server_iterator->second.connection.reset();
                        return 1;
                    };
                    std::istream response(&buffer);
                    std::string line;
                    getline(response, line);
                    boost::trim_right(line);

                    if (line == "DELETED")
                        return 0; // indicate success
                    std::istringstream tokenizer(line);
                    std::string first_token;
                    tokenizer >> first_token;    
                    if ((first_token == "ERROR") || 
                        (first_token == "CLIENT_ERROR") ||
                        (first_token == "SERVER_ERROR"))
                        return 1; // indicate error

                    return 1; // indicate error
                };
        };

        void send_command(std::string const & command, boost::asio::ip::tcp::socket & socket) const {
            boost::asio::write(socket,
                boost::asio::buffer(const_cast<char*>(command.c_str()),
                    sizeof(char) * command.size())
                );
        };
        
        struct connect_impl {
            explicit connect_impl(boost::asio::io_service & service) :
                _service(service) { };
            
            boost::asio::io_service & _service;

            void operator() (typename server_container::value_type & element) {
                if (element.second.connected) return; // skip connected servers
                
                try {
                    connection_ptr new_connection(new boost::asio::ip::tcp::socket(_service));
                    boost::asio::ip::tcp::resolver resolver(_service);
                    boost::asio::ip::tcp::resolver::query query(element.second.host, element.second.port, boost::asio::ip::tcp::resolver::query::numeric_service);
                    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
                    boost::asio::ip::tcp::resolver::iterator end;
                    boost::system::error_code error = boost::asio::error::host_not_found;
                    while (endpoint_iterator != end && error) {
                        boost::optional<boost::system::error_code> timer_result;
                        boost::optional<boost::system::error_code> connect_result;
                        boost::asio::deadline_timer _timer(
                            new_connection->io_service()
                        );
                        
                        new_connection->close();
                        new_connection->async_connect(
                                *endpoint_iterator++,
                                boost::bind(
                                    &boost::optional<boost::system::error_code>::reset,
                                    &connect_result, 
                                    _1
                                    )
                                );
                        
                        _timer.expires_from_now(boost::posix_time::milliseconds(MEMCACHE_TIMEOUT));
                        _timer.async_wait(
                                boost::bind(
                                    &boost::optional<boost::system::error_code>::reset,
                                    &timer_result, 
                                    _1)
                                );
                        
                        new_connection->io_service().reset();

                        while (new_connection->io_service().run_one()) {
                            if (connect_result) {
                                _timer.cancel();
                                error = *connect_result;
                            } else if (timer_result) {
                                new_connection->cancel();
                                if (endpoint_iterator == end)
                                    error = boost::asio::error::timed_out;
                            };
                        };
                    };
                    
                    if (error) {
                        element.second.connected = false;
                        element.second.error = error;
                        element.second.connection.reset();
                        return;
                    };
                    element.second.connected = true;
                    element.second.connection = new_connection;
                } catch (boost::system::system_error & e) {
                    element.second.connected = false;
                    element.second.error = e.code();
                    element.second.connection.reset();
                };
            };
        };

        void validate(std::string const & key) const {
            if (key.find('\r') != std::string::npos) throw invalid_key(key);
            if (key.find('\n') != std::string::npos) throw invalid_key(key);
            if (key.find('\t') != std::string::npos) throw invalid_key(key);
            if (key.find(' ') != std::string::npos) throw invalid_key(key);
        };
    };

    template <class threading_policy = policies::default_threading<>,
        class data_interchange_policy = policies::binary_interchange<>,
        class hash_policy = policies::default_hash<>
    >
    struct basic_request : threading_policy, data_interchange_policy, hash_policy {
        typedef std::map<size_t, std::set<std::string> > key_hash_map;
        typedef basic_handle<threading_policy, data_interchange_policy, hash_policy> handle_type;
        typedef typename handle_type::server_container server_container;
        typedef typename handle_type::pool_container pool_container;
        
        explicit basic_request (handle_type & handle) :
            threading_policy(), data_interchange_policy(), hash_policy(),
            _handle(handle) { };

        template <typename T>
        void get(size_t hash, std::string const & key, T & holder) {
            typename threading_policy::lock scoped_lock(*this);
            callbacks.insert(
                std::pair<const std::string, callback_type>
                    (key, detail::deserializer<T, data_interchange_policy>(holder))
                );
            key_hash[hash].insert(key);
        };

        void perform() {
            std::for_each(key_hash.begin(), key_hash.end(), 
                boost::bind(&basic_request<threading_policy, data_interchange_policy>::perform_impl, this, _1));
        };

        typename handle_type::pool_container::size_type pool_count() {
            return _handle.pools.size();
        };

        private:

        handle_type & _handle;
        mutable key_hash_map key_hash;
        mutable callback_map callbacks;

        void perform_impl(typename key_hash_map::value_type const & element) {
            typename threading_policy::lock scoped_lock(*this);

            std::for_each(element.second.begin(), element.second.end(), 
                boost::bind(
                    &basic_request<threading_policy, data_interchange_policy>::validate, 
                    this, 
                    _1
                    )
                );
            
            size_t offset = element.first;
            if (offset > _handle.servers.size())
                throw offset_out_of_bounds(element.first);

            typename handle_type::connection_container connections;
            bool rehash;
            fusion::tie(connections, rehash) = _handle.get_connections(offset);
            
            typename handle_type::connection_container::iterator
                server_iterator_iterator = connections.begin();

            typename handle_type::connection_container::value_type
                server_iterator = *(connections.begin());
            
            while (server_iterator != _handle.servers.end()) {
                if (server_iterator->second.connected) {
                    std::ostringstream command_stream;
                    command_stream << "get ";
                    std::copy(element.second.begin(), element.second.end(),
                        std::ostream_iterator<std::string>(command_stream, " "));
                    // remove the last space
                    command_stream << "\r\n";
                    
                    boost::asio::streambuf buffer;
                    typename handle_type::connection_ptr connection
                        = server_iterator->second.connection;
                    {
                        try {
                            boost::asio::write(*connection,
                                boost::asio::buffer(const_cast<char *>(command_stream.str().c_str()),
                                    sizeof(char) * command_stream.str().size()));
#ifdef _REENTRANT
                            detail::read_handler handler_instance(*connection,
                                buffer, boost::regex("(END\r\n)|(SERVER_ERROR\\S)|(CLIENT_ERROR\\S)|(ERROR\r\n)"), MEMCACHE_TIMEOUT);
                            handler_instance();
#else
                            boost::asio::read_until(*connection,
                                buffer,
                                boost::regex("(END\r\n)|(SERVER_ERROR\\S)|(CLIENT_ERROR\\S)|(ERROR\\S)"));
#endif
                        } catch (boost::system::system_error & e) {
                            server_iterator->second.error = e.code();
                            server_iterator->second.connected = false;
                            server_iterator->second.connection.reset();
                            if (++server_iterator_iterator != connections.end()) {
                                server_iterator = *server_iterator_iterator;
                                continue;
                            } else {
                                throw key_not_found(command_stream.str());
                            }
                            continue; // try the next iterator
                        };
                    };
                    
                    std::istream response_stream(&buffer);
                    std::string line;
                    std::ostringstream data;
                    
                    while (getline(response_stream, line)) {
                        if (response_stream.eof()) break;
                        data << line << '\n';
                    };

                    std::string data_string = data.str();

                    try {
                        if (!detail::parse_response(data_string, callbacks)) {
                            std::istringstream tokenizer(data_string);
                            std::string first_token;
                            tokenizer >> first_token;
                            if (first_token == "END") {
                                return; // ignore this
                            };
                            if ((first_token == "ERROR") ||
                                (first_token == "SERVER_ERROR") ||
                                (first_token == "CLIENT_ERROR"))
                                throw invalid_response_found(data_string);
                        };
                    } catch (typename data_interchange_policy::archive_exception & e) {
                        std::ostringstream malformed_data_stream;
                        malformed_data_stream << command_stream.str()
                            << data_string;
                        throw malformed_data(malformed_data_stream.str());
                    };
                };
                ++server_iterator;
            };
            
        };

        void validate(std::string const & key) const {
            if (key.find('\r') != std::string::npos) throw invalid_key(key);
            if (key.find('\n') != std::string::npos) throw invalid_key(key);
            if (key.find('\t') != std::string::npos) throw invalid_key(key);
            if (key.find(' ') != std::string::npos) throw invalid_key(key);
        };
    };

    template <class threading_policy, class data_interchange_policy , class directive_type>
    inline basic_handle<threading_policy, data_interchange_policy> & operator<< (basic_handle<threading_policy, data_interchange_policy> & _handle, directive_type const & directive) {
        directive(_handle);
        return _handle;
    };

    template <class threading_policy, class data_interchange_policy , class hash_policy, class directive_type>
    inline basic_request<threading_policy, data_interchange_policy, hash_policy> & operator<< (basic_request<threading_policy, data_interchange_policy, hash_policy> & _request, directive_type const & directive) {
        directive(_request);
        return _request;
    };

    template <typename threading_policy, class data_interchange_policy>
    inline basic_handle<threading_policy, data_interchange_policy> & operator<< (basic_handle<threading_policy, data_interchange_policy> & _handle, tags::connect) {
        _handle.connect();
        return _handle;
    };

    template <typename threading_policy, class data_interchange_policy, class hash_policy>
    inline basic_request<threading_policy, data_interchange_policy, hash_policy> & operator<< (basic_request<threading_policy, data_interchange_policy, hash_policy> & _request, tags::commit) {
        _request.perform();
        return _request;
    };

#ifndef _REENTRANT
    typedef basic_handle<> handle ;
    typedef basic_request<> request ;
#else
    typedef basic_handle<policies::boost_threading<> > handle ;
    typedef basic_request<policies::boost_threading<> > request ;
#endif

    extern tags::connect connect ;

    extern tags::commit commit ;

};

#endif

