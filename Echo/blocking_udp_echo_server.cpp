/**

async_udp_echo_server.cpp

Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)

Distributed under the Boost software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

*/

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>

enum { max_length = 1024 };

void Server(boost::asio::io_context& io_context, unsigned short port)
{
  boost::asio::ip::udp::socket sock(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
  for (;;)
  {
    char data_[max_length];
    boost::asio::ip::udp::endpoint sender_endpoint_;
    std::size_t length = sock.receive_from(boost::asio::buffer(data_, max_length), sender_endpoint_);
    sock.send_to(boost::asio::buffer(data_, length), sender_endpoint_);
  }
}

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usgae: blocking_udp_echo_server <port> \n";
      return 1;
    }

    boost::asio::io_context io_context;
    Server(io_context, std::atoi(argv[1]));
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
