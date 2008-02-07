
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __MEMCACHE_DETAIL_DIRECTIVES_POOL_HPP__
#define __MEMCACHE_DETAIL_DIRECTIVES_POOL_HPP__

#include <boost/fusion/algorithm/iteration/for_each.hpp>

#include "server.hpp"
#include "../../tags.hpp"
#include "../../server_pool.hpp"
#include <list>
#include <boost/cast.hpp>
#include <algorithm>

namespace memcache { namespace detail {

    namespace fusion = boost::fusion ;

    namespace impl {
        typedef std::set<std::string> server_set;
        template <class handle_type>
            struct add_servers {
                explicit add_servers(handle_type & handle, server_set & servers)
                    : _handle(handle), _servers(servers)
                    { };
                
                template <typename T>
                    void operator() (T const & element) const {
                        typename handle_type::server_info s_info 
                            = { false, element._server_name, element._port };
                        
                        std::string servername = (element._server_name + ':') + element._port;
                        _servers.insert(servername);
                        _handle.add_server(servername, s_info);
                    };

                handle_type & _handle;
                server_set & _servers;
            };
    };

    template <class tuple_type>
        struct pool_directive {
            mutable std::string _name;
            mutable int _status;
            mutable tuple_type _tuple;

            explicit pool_directive(std::string const & name, int status, tuple_type const & tuple_)
                : _name(name), _status(status), _tuple(tuple_)
                { };
            
            template <class handle_type>
                void operator() (handle_type & handle) const {
                    impl::server_set servers;
                    impl::add_servers<handle_type> server_adder(handle, servers);
                    fusion::for_each(
                            _tuple,
                            server_adder
                            );
                    
                    typename handle_type::pool_info p_info 
                        = { _status, servers };
                    
                    handle.add_pool(_name, p_info);
                };
        };

    template <>
        struct pool_directive<server_pool> {
            mutable server_pool & _pool;
            explicit pool_directive(server_pool & pool)
                : _pool(pool)
                { };
            
            template <class handle_type>
                void operator() (handle_type & handle) const {
                    impl::server_set servers;
                    impl::add_servers<handle_type> server_adder(handle, servers);
                    std::for_each(
                            _pool.servers.begin(), 
                            _pool.servers.end(), 
                            server_adder
                            )
                        ;
                    
                    typename handle_type::pool_info p_info 
                        = { _pool.status, servers };
                    
                    handle.add_pool(_pool.name, p_info);
                };
        };
    
}; // namespace detail

    template <typename T, typename _T>
    inline detail::pool_directive<_T> pool(T _name, int status, _T servers) {
        return detail::pool_directive<_T>(std::string(_name), status, servers);
    };

    inline detail::pool_directive<server_pool> pool(server_pool & pool_) {
        return detail::pool_directive<server_pool>(pool_);
    };

    template <typename T, typename _T>
    inline detail::pool_directive<_T> pool(T _name, _T servers) {
        return detail::pool_directive<_T>(std::string(_name), 3, servers);
    };

}; // namespace memcache

#endif // __MEMCACHE_DETAIL_DIRECTIVES_POOL_HPP__

