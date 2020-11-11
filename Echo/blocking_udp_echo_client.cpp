/**

async_udp_echo_server.cpp

Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)

Distributed under the Boost software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

*/

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "usage: blocking_udp_echo_clinet <host> <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    boost::asio::ip::udp::socket socket(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));

    boost::asio::ip::udp::resolver resolver(io_context);
    boost::asio::ip::udp::resolver::results_type endpoints = resolver.resolve(boost::asio::ip::udp::v4(), argv[1], argv[2]);

    std::cout << "Enter message: ";
    char request[max_length];
    std::cin.getline(request, max_length);
    std::size_t request_length = std::strlen(request);
    socket.send_to(boost::asio::buffer(request, request_length), *endpoints.begin());

    char reply[max_length];
    boost::asio::ip::udp::endpoint sender_endpoint_;
    std::size_t reply_length = socket.receive_from(boost::asio::buffer(reply, max_length), sender_endpoint_);
    std::cout << "Reply is: ";
    std::cout.write(reply, reply_length);
    std::cout << "\n";
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
