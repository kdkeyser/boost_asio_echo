//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include "exit_matcher.h"
#include <vector>
#include <boost/bind.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

namespace echo {
  namespace server {

    connection::connection(boost::asio::io_service& io_service)
    : strand_(io_service), socket_(io_service), em () {
    }

    boost::asio::ip::tcp::socket& connection::socket() {
      return socket_;
    }

    void connection::start() {
      socket_.async_read_some(boost::asio::buffer(buffer_),
              strand_.wrap(
              boost::bind(&connection::handle_read, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred)));
    }

    void connection::handle_read(const boost::system::error_code& e,
            std::size_t bytes_transferred) {
      if (!e) {
        /* Check if we got an exit command. */
        if (em.try_match(buffer_, bytes_transferred)) {
          std::cerr << "Saw exit: terminating connection" << std::endl;
          socket_.close ();
          
          return;
        }
        
        /* Echo the data back */
        boost::asio::async_write(socket_, boost::asio::buffer(buffer_, bytes_transferred),
                strand_.wrap(
                boost::bind(&connection::handle_write, shared_from_this(),
                boost::asio::placeholders::error)));

        return;
      } else {
        std::cerr << "Could not read data: " << e << std::endl;
        socket_.close();
        return;
      }
    }

    void connection::handle_write(const boost::system::error_code& e) {
      if (!e) {
        /* Read some more data to echo. */
        socket_.async_read_some(boost::asio::buffer(buffer_),
                strand_.wrap(
                boost::bind(&connection::handle_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred)));

        return;
      }
      else {
        std::cerr << "Could not write data: " << e << std::endl;
        socket_.close();
        return;
      }
    }

  } // namespace server3
} // namespace http
