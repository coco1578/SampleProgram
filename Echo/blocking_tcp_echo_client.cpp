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
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    boost::asio::ip::tcp::socket socket_(io_context);
    boost::asio::ip::tcp::resolver resolver_(io_context);
    boost::asio::connect(socket_, resolver_.resolve(argv[1], argv[2]));

    std::cout << "Enter message: ";
    char request[max_length];
    std::cin.getline(request, max_length);
    std::size_t request_length = std::strlen(request);
    boost::asio::write(socket_, boost::asio::buffer(request, request_length));

    char reply[max_length];
    std::size_t reply_length = boost::asio::read(socket_, boost::asio::buffer(reply, request_length));
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
