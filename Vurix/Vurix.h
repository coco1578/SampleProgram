#pragma once
#define CURL_STATICLIB
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <curl/curl.h>

class HttpResponse
{
public:
  HttpResponse() : code(404)
  {
  }
  virtual ~HttpResponse()
  {
  }

  int code;
  std::string header;
  std::string data;
};

std::size_t write_callback(char* ptr, std::size_t size, std::size_t nmemb, void* userdata);

HttpResponse Login(std::string& url, std::string& id, std::string& password, std::string& group, std::string& license);
HttpResponse Logout(std::string& url, std::string& auth_token);