/**

async_tcp_echo_server.cpp

Copyright (c) 2003-2020 Christopher M. Kohlhof (chris at kholhoff dot com)

Distructed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef CHAT_MESSAGE_HPP_
#define CHAT_MESSAGE_HPP_

#include <cstdio>
#include <cstdlib>
#include <cstring>

class ChatMessage
{
public:
  enum { header_length = 4 };
  enum { max_body_length = 512 };

  ChatMessage() : body_length_(0)
  {
  }

  const char* Data() const
  {
    return data_;
  }

  char* Data()
  {
    return data_;
  }

  std::size_t Length() const
  {
    return header_length + body_length_;
  }

  const char* Body() const
  {
    return data_ + header_length;
  }

  char* Body()
  {
    return data_ + header_length;
  }

  std::size_t BodyLength() const
  {
    return body_length_;
  }

  void BodyLength(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool DecodeHeader()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, header_length);
    body_length_ = std::atoi(header);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void EncodeHeader()
  {
    char header[header_length + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
  }

private:

  char data_[header_length + max_body_length];
  std::size_t body_length_;

};


#endif