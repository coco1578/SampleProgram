/**

async_tcp_echo_server.cpp

Copyright (c) 2003-2020 Christopher M. Kohlhof (chris at kholhoff dot com)

Distructed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "chat_message.hpp"

typedef std::deque<ChatMessage> chat_message_queue;

class ChatParticipant
{
public:
  virtual ~ChatParticipant()
  {
  }
  virtual void deliver(const ChatMessage& msg) = 0;
};

typedef std::shared_ptr<ChatParticipant> chat_participant_ptr;

class ChatRoom
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg : recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const ChatMessage& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant : participants_)
      participant->deliver(msg);
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};

class ChatSession : public ChatParticipant, public std::enable_shared_from_this<ChatSession>
{
public:
  ChatSession(boost::asio::ip::tcp::socket socket, ChatRoom& room) : socket_(std::move(socket)), room_(room)
  {
  }

  void start()
  {
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const ChatMessage& msg)
  {
    bool write_in_progress = !write_msg_.empty();
    write_msg_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:

  void do_read_header()
  {
    auto self(shared_from_this());
    boost::asio::async_read(socket_, boost::asio::buffer(read_msg_.Data(), ChatMessage::header_length),
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      if (!ec && read_msg_.DecodeHeader())
      {
        do_read_body();
      }
      else
      {
        room_.leave(shared_from_this());
      }
    });
  }

  void do_read_body()
  {
    auto self(shared_from_this());
    boost::asio::async_read(socket_, boost::asio::buffer(read_msg_.Body(), read_msg_.BodyLength()),
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      if (!ec)
      {
        room_.deliver(read_msg_);
        do_read_header();
      }
      else
      {
        room_.leave(shared_from_this());
      }
    });
  }

  void do_write()
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(write_msg_.front().Data(), write_msg_.front().Length()),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      if (!ec)
      {
        write_msg_.pop_front();
        if (!write_msg_.empty())
        {
          do_write();
        }
      }
      else
      {
        room_.leave(shared_from_this());
      }
    });
  }

private:
  boost::asio::ip::tcp::socket socket_;
  ChatRoom& room_;
  ChatMessage read_msg_;
  chat_message_queue write_msg_;
};

class ChatServer
{
public:
  ChatServer(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint) : acceptor_(io_context, endpoint)
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
        std::make_shared<ChatSession>(std::move(socket), room_)->start();
      }

      do_accept();
    });
  }

  boost::asio::ip::tcp::acceptor acceptor_;
  ChatRoom room_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_context io_context;

    std::list<ChatServer> servers;
    for (int i = 1; i < argc; ++i)
    {

    }
  }
}
