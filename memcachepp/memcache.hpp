
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
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
#include <numeric>
#include <istream>
#include <sstream>

#include <memcachepp/memcache/detail/directives.hpp>
#include <memcachepp/memcache/detail/read_handler.hpp>
#include <memcachepp/memcache/detail/deserializer.hpp>
#include <memcachepp/memcache/exceptions.hpp>
#include <memcachepp/memcache/tags.hpp>
#include <memcachepp/memcache/server_pool.hpp>
#include <memcachepp/memcache/parser.hpp>
#include <memcachepp/memcache/policies.hpp>
#include <memcachepp/memcache/handle_concept.hpp>
#include <memcachepp/memcache/handle.hpp>

// Include the fluent interface
#include <memcachepp/memcache/fluent.hpp>

#include <boost/fusion/tuple.hpp>

namespace memcache {
    
    typedef boost::function<void(std::string const &)> 
        callback_type;
    
    typedef std::map<std::string, callback_type> 
        callback_map;

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

    template <class threading_policy, class data_interchange_policy, class hash_policy, class directive_type>
    inline basic_handle<threading_policy, data_interchange_policy, hash_policy> & operator<< (basic_handle<threading_policy, data_interchange_policy, hash_policy> & _handle, directive_type const & directive) {
        directive(_handle);
        return _handle;
    };

    template <class threading_policy, class data_interchange_policy , class hash_policy, class directive_type>
    inline basic_request<threading_policy, data_interchange_policy, hash_policy> & operator<< (basic_request<threading_policy, data_interchange_policy, hash_policy> & _request, directive_type const & directive) {
        directive(_request);
        return _request;
    };

    template <typename threading_policy, class data_interchange_policy, class hash_policy>
    inline basic_handle<threading_policy, data_interchange_policy, hash_policy> & operator<< (basic_handle<threading_policy, data_interchange_policy, hash_policy> & _handle, connect_directive_t) {
        _handle.connect();
        return _handle;
    };

    template <typename threading_policy, class data_interchange_policy, class hash_policy>
    inline basic_request<threading_policy, data_interchange_policy, hash_policy> & operator<< (basic_request<threading_policy, data_interchange_policy, hash_policy> & _request, commit_directive_t) {
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

    BOOST_CONCEPT_ASSERT((Handle<handle>));

};

#endif

