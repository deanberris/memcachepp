
// Copyright 2007, 2008 (c) Friendster, Inc.
// Copyright 2007, 2008 (c) Dean Michael Berris <dmberris@friendster.com>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEMCACHE_DETAIL_READ_HANDLER_HPP__
#define MEMCACHE_DETAIL_READ_HANDLER_HPP__

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#ifdef _REENTRANT
#include <boost/optional.hpp>
#endif

#include <memcachepp/memcache/tags.hpp>
#include <boost/regex.hpp>

namespace memcache { namespace detail {

#ifdef _REENTRANT
    template <class tag = tags::default_tag>
    struct read_handler_impl {
        explicit read_handler_impl(
            boost::asio::ip::tcp::socket & socket, 
            boost::asio::streambuf & buffer,
            boost::regex const & pattern,
            time_t timeout)
            : _socket(socket), 
            _buffer(buffer), _pattern(pattern), _timeout(timeout)
            { 
            };

        boost::asio::ip::tcp::socket & _socket;
        boost::asio::streambuf & _buffer;
        boost::regex _pattern;
        time_t _timeout;

        static void set_result(boost::optional<boost::system::error_code> *a, boost::system::error_code code) {
            a->reset(code);
        };

        void operator() () {
            boost::optional<boost::system::error_code> timer_result;
            boost::optional<boost::system::error_code> read_result;
            boost::asio::async_read_until(_socket, _buffer, _pattern, boost::bind(&read_handler_impl<tag>::set_result, & read_result, _1));
            boost::asio::deadline_timer _timer(_socket.io_service());
            _timer.expires_from_now(boost::posix_time::milliseconds(_timeout));
            _timer.async_wait(boost::bind(&read_handler_impl<tag>::set_result, & timer_result , _1));
            _socket.io_service().reset();

            while (_socket.io_service().run_one()) {
                if (read_result)
                    _timer.cancel();
                else if (timer_result)
                    _socket.cancel();
            }

            if (*read_result)
                throw boost::system::system_error(*read_result);
        };

    };
    
    typedef read_handler_impl<> read_handler ;

#endif // _REENTRANT

} // namespace detail

} // namespace memcache

#endif // MEMCACHE_DETAIL_READ_HANDLER_HPP__

