#ifndef MEMCACHEPP_MEMCACHE_REQUEST_HPP_
#define MEMCACHEPP_MEMCACHE_REQUEST_HPP_

// Copyright 2009 (c) Dean Michael Berris <mikhailberis@gmail.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

namespace memcache {

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
        }

        void perform() {
            std::for_each(key_hash.begin(), key_hash.end(), 
                boost::bind(&basic_request<threading_policy, data_interchange_policy>::perform_impl, this, _1));
        };

        typename handle_type::pool_container::size_type pool_count() {
            return _handle.pools.size();
        };

        private:

        handle_type & _handle;

        typedef boost::function<void(std::string const &)> 
            callback_type;
        
        typedef std::map<std::string, callback_type> 
            callback_map;

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
            boost::fusion::tie(connections, rehash) = _handle.get_connections(offset);
            
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
                        boost::uint64_t cas_value = 0u;
                        if (!detail::parse_response(data_string, callbacks, cas_value)) {
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

    template <class threading_policy, class data_interchange_policy , class hash_policy, class directive_type>
    inline basic_request<threading_policy, data_interchange_policy, hash_policy> & operator<< (basic_request<threading_policy, data_interchange_policy, hash_policy> & _request, directive_type const & directive) {
        directive(_request);
        return _request;
    }

    template <typename threading_policy, class data_interchange_policy, class hash_policy>
    inline basic_request<threading_policy, data_interchange_policy, hash_policy> & operator<< (basic_request<threading_policy, data_interchange_policy, hash_policy> & _request, commit_directive_t) {
        _request.perform();
        return _request;
    }

} // namespace memcache

#endif

