/**

async_tcp_echo_server.cpp

Copyright (c) 2003-2020 Christopher M. Kohlhof (chris at kholhoff dot com)

Distructed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/


#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>


class Session : public std::enable_shared_from_this<Session>
{
public:
  Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket))
  {
  }

  void start()
  {
    do_read();
  }

private:

  void do_read()
  {
    auto self(shared_from_this()); // std::shared_ptr<Session> self;
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      if (!ec)
      {
        do_read();
      }
    });
  }

  void do_write(std::size_t length)
  {
    auto self(shared_from_this()); // std::shared_ptr<Session> self;
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      if (!ec)
      {
        do_read();
      }
    });
  }

  boost::asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class Server
{
public:

  Server(boost::asio::io_context& io_context, short port) : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
  {
    do_accept();
  }

private:

  void do_accept()
  {
    acceptor_.async_accept(
    [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
    {
      if (!ec)
      {
        std::make_shared<Session>(std::move(socket))->start();
      }

      do_accept();
    });
  }

  boost::asio::ip::tcp::acceptor acceptor_;
};


int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    Server s(io_context, std::atoi(argv[1]));
    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}