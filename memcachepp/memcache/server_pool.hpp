
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_SERVER_POOL_HPP__
#define __MEMCACHE_SERVER_POOL_HPP__

#include <memcachepp/memcache/tags.hpp>
#include <list>

namespace memcache {
    template <class tag = tags::container>
        struct server_pool_impl {
            struct server_name_port_pair {
                std::string _server_name, _port;
            };
            typedef std::list<server_name_port_pair> server_list;
            
            std::string name;
            int status;
            server_list servers;
            
            explicit server_pool_impl(std::string const & pool_name, int pool_status = 3) 
                : name(pool_name), status(pool_status)
                { };

            void add_server(std::string const & server_name, unsigned short int port) {
                server_name_port_pair server_and_port = 
                { server_name, boost::lexical_cast<std::string>(port) };
                servers.push_back(server_and_port);
            };

        };

    typedef server_pool_impl<> server_pool;
};

#endif // __MEMCACHE_SERVER_POOL_HPP__

