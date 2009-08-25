#ifndef MEMCACHEPP_MEMCACHE_HANDLE_HPP_
#define MEMCACHEPP_MEMCACHE_HANDLE_HPP_

// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
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
#include <boost/cstdint.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

namespace memcache {

    template <
        class threading_policy,
        class data_interchange_policy,
        class hash_policy
    >
    struct basic_request ; // forward declaration

    using std::map;
    using std::vector;
    using std::string;
    using std::pair;
    using std::ostringstream;
    using std::istringstream;
    using std::istream;
    using std::make_pair;
    using boost::system::error_code;
    using boost::system::system_error;
    using boost::fusion::tuple;
    using boost::fusion::tie;
    using boost::fusion::make_tuple;
    
    template <
        class threading_policy = policies::default_threading<>, 
        class data_interchange_policy = policies::binary_interchange<>,
        class hash_policy = policies::default_hash<>
    >
    struct basic_handle : 
        threading_policy, 
        data_interchange_policy, 
        hash_policy
    {

        typedef boost::shared_ptr<boost::asio::ip::tcp::socket> connection_ptr;
        
        struct server_info {
            bool connected;
            string host, port;
            error_code error;
            connection_ptr connection;
        };

        struct pool_info {
            enum { standalone = 0,
                replicating = 1,
                failover = 2 };
            typedef std::set<string> member_container;

            int status;
            member_container members;
        };

        typedef map<string, server_info> server_container;
        typedef map<string, pool_info> pool_container;
        
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
        void set(size_t offset, string const & key, T const & value, time_t expiration, time_t failover_expiration, boost::uint16_t flags = 0) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            tie(connections, rehash) = get_connections(offset);
            size_t connections_size = connections.size();

            vector<int> errors(distance(connections.begin(), connections.end()), 0);

            transform(connections.begin(), connections.end(),
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
                    static_cast<size_t>(
                        accumulate(
                            errors.begin(), 
                            errors.end(), 
                            0
                        )
                    )
                )
                throw key_not_stored(key);
        };

        void set_raw(size_t offset, string const & key, string const & value, time_t expiration, time_t failover_expiration, boost::uint16_t flags = 0) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            tie(connections, rehash) = get_connections(offset);
            size_t connections_size = connections.size();

            vector<int> errors(distance(connections.begin(), connections.end()), 0);

            transform(connections.begin(), connections.end(),
                    errors.begin(),
                    set_impl<string, policies::string_preserve>(
                        key, 
                        value, 
                        expiration, 
                        failover_expiration, 
                        flags, 
                        rehash
                        )
                    );

            if (connections_size == 
                    static_cast<size_t>(accumulate(errors.begin(), errors.end(), 0)))
                throw key_not_stored(key);
        };

        void delete_(size_t offset, string const & key, time_t delay = 0) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            tie(connections, rehash) = get_connections(offset);
            size_t connections_size = connections.size();

            vector<int> errors(distance(connections.begin(), connections.end()), 0);
            transform(connections.begin(), connections.end(),
                    back_inserter(errors),
                    delete_impl(
                        key, 
                        delay
                        )
                    );

            if (connections_size ==
                    static_cast<size_t>(
                        accumulate(
                            errors.begin(), 
                            errors.end(), 
                            0
                            )
                        )
               )
                throw key_not_found(key);
        };

        template <typename T> // T must be serializable
        void get(size_t offset, string const & key, T & holder) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            tie(connections, rehash) = get_connections(offset);

            typename connection_container::iterator 
                connection_iterator = 
                    connections.begin(),
                connections_end =
                    connections.end();

            get_impl<T, data_interchange_policy>
                getter(key, holder);

            connection_iterator =
                find_if(connection_iterator, connections_end, getter);
            
            // when you get here, it means you didn't find the key
            if (connection_iterator == connections_end)
                throw key_not_found(key);
        };

        void get_raw(size_t offset, string const & key, string & holder) {
            typename threading_policy::lock scoped_lock(*this);
            validate(key);
            if (offset > pools.size())
                throw offset_out_of_bounds(offset);

            connection_container connections;
            bool rehash;
            tie(connections, rehash) = get_connections(offset);

            typename connection_container::iterator 
                connection_iterator = 
                    connections.begin(),
                connections_end =
                    connections.end();

            get_impl<string, policies::string_preserve>
                getter(key, holder);

            connection_iterator =
                find_if(connection_iterator, connections_end, getter);
            
            // when you get here, it means you didn't find the key
            if (connection_iterator == connections_end)
                throw key_not_found(key);
        };
        
        bool is_connected(string const & server_name) { 
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

        friend struct basic_request<threading_policy, data_interchange_policy, hash_policy>;

        typename server_container::size_type server_count() {
            return servers.size();
        };

        typename pool_container::size_type pool_count() {
            return pools.size();
        };

        void add_server(string const & server_name, server_info const & s_info) {
            servers.insert(
                    make_pair(server_name, s_info)
                    );
        };

        void add_pool(string const & pool_name, pool_info p_info) {
            pools.insert(
                    make_pair(pool_name, p_info)
                    );
        };

        private:
        
        server_container servers;
        pool_container pools;

        typedef boost::function<void(string const &)> 
            callback_type;
        
        typedef map<string, callback_type> 
            callback_map;

        typedef vector<typename server_container::iterator> connection_container;

        inline tuple <connection_container, bool> get_connections(size_t offset) {
            typename pool_container::iterator pool_iterator;
            size_t counter = 0;
            for (pool_iterator = pools.begin();
                    pool_iterator != pools.end() && counter < offset;
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
                BOOST_FOREACH (string const & server_name, pool_iterator->second.members) {
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
                    BOOST_FOREACH (string server_name, pool_iterator->second.members) {
                        typename server_container::iterator server_iterator
                            = servers.find(server_name);
                        if (server_iterator->second.connected)
                            connections.push_back(server_iterator);
                    };
                };
            };
            
            return make_tuple(connections, rehash);
        };

        boost::asio::io_service _service;

        template <typename T, typename set_interchange_policy>
            struct set_impl {
                string command;
                
                explicit set_impl(string const & key, T const & value, time_t expiration, time_t failover_expiration, boost::uint16_t flags, bool rehash) 
                    { 
                        ostringstream output_bytes_stream;
                        typename set_interchange_policy::oarchive archive(output_bytes_stream);
                        archive << value;

                        ostringstream command_stream;
                        command_stream << "set " << key << " " << flags
                            << " " << (rehash ? failover_expiration : expiration)
                            << " " << output_bytes_stream.str().size() << "\r\n"
                            << output_bytes_stream.str() << "\r\n";
                        command = command_stream.str();
                    };
                
                template <typename value_type>
                    int operator() (value_type & server_iterator) {
                        static boost::regex eol_regex("\r\n");
                        boost::asio::streambuf buffer;
                        connection_ptr connection = server_iterator->second.connection;
                        try {
                            boost::asio::write(*connection,
                                boost::asio::buffer(const_cast<char*>(command.c_str()),
                                    sizeof(char) * command.size())
                                );
#ifdef _REENTRANT
                            detail::read_handler handler_instance(*connection, buffer,
                                eol_regex, MEMCACHE_TIMEOUT);
                            handler_instance();
#else
                            boost::asio::read_until(*connection,
                                buffer,
                                eol_regex);
#endif
                        } catch (system_error & e) {
                            server_iterator->second.error = e.code();
                            server_iterator->second.connected = false;
                            server_iterator->second.connection.reset();
                            return 1; // indicate error
                        };

                        istream response(&buffer);
                        string line;
                        getline(response, line);

                        if (line == "STORED\r")
                            return 0; // indicate NO error

                        istringstream tokenizer(line);
                        string first_token;
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
                string _key;
                holder_type & _holder;
                string command;
                explicit get_impl(string const & key, holder_type & holder)
                    : _key(key), _holder(holder) 
                    {
                        ostringstream command_stream;
                        command_stream << "get " << key << "\r\n";
                        command = command_stream.str();
                    };
                
                template <typename T>
                    bool operator() (T & server_iterator) {
                        static boost::regex end_indicator("(END\r\n)|(SERVER_ERROR\\S)|(CLIENT_ERROR\\S)|(ERROR\r\n)");
                        boost::asio::streambuf buffer;
                        connection_ptr connection = server_iterator->second.connection;
                        try {
                            boost::asio::write(*connection,
                                boost::asio::buffer(const_cast<char*>(command.c_str()),
                                    sizeof(char) * command.size())
                                );
#ifdef _REENTRANT
                            detail::read_handler handler_instance(
                                    *connection, 
                                    buffer, 
                                    end_indicator,
                                    MEMCACHE_TIMEOUT
                                    );
                            handler_instance();
#else
                            boost::asio::read_until(*connection,
                                buffer,
                                end_indicator
                            );
#endif
                        } catch (system_error & e) {
                            server_iterator->second.error = e.code();
                            server_iterator->second.connected = false;
                            server_iterator->second.connection.reset();
                            return false; // wasn't able to get it
                        };

                        istream response(&buffer);
                        
                        callback_map callbacks;
                        callbacks[_key] = detail::deserializer<holder_type, get_interchange_policy>(_holder);
                        
                        ostringstream data;
                        string line;
                        while (getline(response, line)) {
                            if (response.eof()) break;
                            data << line << '\n';
                        };

                        string data_string(data.str());

                        try {
                            if (!detail::parse_response(data_string, callbacks)) {
                                istringstream tokenizer(data_string);
                                string first_token;
                                tokenizer >> first_token;
                                if (first_token == "END")
                                    return false; // wasn't able to get it
                                if ((first_token == "ERROR") || 
                                    (first_token == "CLIENT_ERROR") ||
                                    (first_token == "SERVER_ERROR"))
                                    throw invalid_response_found(data_string);
                            };
                        } catch (typename data_interchange_policy::archive_exception & e) {
                            ostringstream malformed_data_stream;
                            malformed_data_stream << command
                                << data_string;
                            throw malformed_data(malformed_data_stream.str());
                        };

                        return true;
                    };
                };
        
        struct delete_impl {
            string command;
            explicit delete_impl(string const & key, time_t delay)
                {
                    ostringstream command_stream;
                    command_stream << "delete " << key << ' ' << delay << "\r\n";
                    command = command_stream.str();
                };

            template <typename value_type>
                int operator() (value_type & server_iterator) {
                    static boost::regex eol_regex("\r\n");
                    boost::asio::streambuf buffer;
                    connection_ptr connection = server_iterator->second.connection;
                    try {
                        boost::asio::write(*connection,
                            boost::asio::buffer(const_cast<char*>(command.c_str()),
                                sizeof(char) * command.size())
                            );
#ifdef _REENTRANT
                            detail::read_handler handler_instance(*connection,
                                buffer, eol_regex, MEMCACHE_TIMEOUT);
                            handler_instance();
#else
                            boost::asio::read_until(*connection,
                                buffer, eol_regex);
#endif
                    } catch (system_error & e) {
                        server_iterator->second.error = e.code();
                        server_iterator->second.connected = false;
                        server_iterator->second.connection.reset();
                        return 1;
                    };
                    istream response(&buffer);
                    string line;
                    getline(response, line);
                    boost::trim_right(line);

                    if (line == "DELETED")
                        return 0; // indicate success
                    istringstream tokenizer(line);
                    string first_token;
                    tokenizer >> first_token;    
                    if ((first_token == "ERROR") || 
                        (first_token == "CLIENT_ERROR") ||
                        (first_token == "SERVER_ERROR"))
                        return 1; // indicate error

                    return 1; // indicate error
                };
        };

        void send_command(string const & command, boost::asio::ip::tcp::socket & socket) const {
            boost::asio::write(socket,
                boost::asio::buffer(const_cast<char*>(command.c_str()),
                    sizeof(char) * command.size())
                );
        };
        
        struct connect_impl {
            explicit connect_impl(boost::asio::io_service & service) :
                _service(service) { };
            
            boost::asio::io_service & _service;

            template <class IoService, class Element>
                pair<connection_ptr,error_code> 
                connect(IoService & service_, Element const & element) const {
                    using boost::asio::ip::tcp;
                    using boost::optional;
                    using boost::asio::deadline_timer;
                    using boost::bind;
                    using boost::posix_time::milliseconds;
                    using boost::asio::error::timed_out;
                    using boost::asio::error::host_not_found;

                    connection_ptr new_connection(
                            new tcp::socket(_service)
                            );
                    tcp::resolver resolver(_service);
                    tcp::resolver::query query(element.second.host, element.second.port, tcp::resolver::query::numeric_service);
                    tcp::resolver::iterator 
                        endpoint_iterator = resolver.resolve(query),
                        end;
                    error_code error = host_not_found;
                    while (endpoint_iterator != end && error) {
                        optional<error_code> timer_result;
                        optional<error_code> connect_result;
                        deadline_timer _timer(
                            new_connection->io_service()
                        );
                        
                        new_connection->close();
                        new_connection->async_connect(
                                *endpoint_iterator++,
                                bind(
                                    &optional<error_code>::reset,
                                    &connect_result,
                                    _1
                                    )
                                );
                        _timer.expires_from_now(
                                milliseconds(MEMCACHE_TIMEOUT)
                                );
                        _timer.async_wait(
                                bind(
                                    &optional<error_code>::reset,
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

                    return make_pair(new_connection, error);
                }

            void operator() (typename server_container::value_type & element) {
                if (element.second.connected) return; // skip connected servers
                
                try {
                    connection_ptr new_connection;
                    error_code error;
                    
                    tie(new_connection, error) = connect(_service, element);
                    
                    if (error) {
                        element.second.connected = false;
                        element.second.error = error;
                        element.second.connection.reset();
                    } else {
                        element.second.connected = true;
                        element.second.connection = new_connection;
                    }
                } catch (system_error & e) {
                    element.second.connected = false;
                    element.second.error = e.code();
                    element.second.connection.reset();
                };
            };
        };

        void validate(string const & key) const {
            if (key.find('\r') != string::npos) throw invalid_key(key);
            if (key.find('\n') != string::npos) throw invalid_key(key);
            if (key.find('\t') != string::npos) throw invalid_key(key);
            if (key.find(' ') != string::npos) throw invalid_key(key);
        };
    };

} // namespace memcache

#endif

