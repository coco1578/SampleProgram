#include <iostream>

#include "Vurix.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"

std::size_t write_callback(char* ptr, std::size_t size, std::size_t nmemb, void* userdata)
{
  std::string* data = (std::string*)userdata;
  *data = ptr;
  return data->size();
}


HttpResponse Login(std::string& url, std::string& id, std::string& password, std::string& group, std::string& license)
{
  HttpResponse resp;
  struct curl_slist* chunk = NULL;

  auto libcurl = curl_easy_init();
  if (libcurl)
  {
    /* Vurix Login Header */
    std::string id_ = "x-account-id: " + id;
    std::string password_ = "x-account-pass: " + password;
    std::string group_ = "x-account-group: " + group;
    std::string license_ = "x-license: " + license;
    chunk = curl_slist_append(chunk, id_.c_str());
    chunk = curl_slist_append(chunk, password_.c_str());
    chunk = curl_slist_append(chunk, group_.c_str());
    chunk = curl_slist_append(chunk, license_.c_str());

    curl_easy_setopt(libcurl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(libcurl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(libcurl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(libcurl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(libcurl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(libcurl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(libcurl, CURLOPT_WRITEDATA, &resp.data);

    auto ret = curl_easy_perform(libcurl);
    if (ret != CURLE_OK)
    {
      // Failed
    }
  }
  curl_easy_getinfo(libcurl, CURLINFO_RESPONSE_CODE, &resp.code);
  curl_easy_cleanup(libcurl);

  return resp;
}

HttpResponse Logout(std::string& url, std::string& auth_token)
{
  HttpResponse resp;

  struct curl_slist* chunk = NULL;

  auto libcurl = curl_easy_init();
  if (libcurl)
  {
    /* Vurix Logout Header */
    std::string auth_token_ = "x-auth-token: " + auth_token;

    chunk = curl_slist_append(chunk, auth_token_.c_str());

    curl_easy_setopt(libcurl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(libcurl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(libcurl, CURLOPT_TIMEOUT, 3L);
    curl_easy_setopt(libcurl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(libcurl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(libcurl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(libcurl, CURLOPT_WRITEDATA, &resp.data);

    auto ret = curl_easy_perform(libcurl);
    if (ret != CURLE_OK)
    {
      // Failed
    }
  }

  curl_easy_getinfo(libcurl, CURLINFO_RESPONSE_CODE, &resp.code);
  curl_easy_cleanup(libcurl);

  return resp;
}

int main()
{
  std::string login_url = "";
  std::string id = "";
  std::string password = "";
  std::string group = "";
  std::string license = "";
  std::string auth_token = "";
  HttpResponse login_resp;
  rapidjson::Document login_document;
  

  login_resp = Login(login_url, id, password, group, license);
  if (login_resp.code == 200)
  {
    login_document.Parse(login_resp.data.c_str());

    if (login_document.HasMember("results"))
    {
      const rapidjson::Value& results = login_document["results"];
      if (results.HasMember("auth_token"))
      {
        auth_token = results["auth_token"].GetString();
        std::cout << auth_token << std::endl;
      }
    }
  }

  std::string logout_url = "";
  HttpResponse logout_resp;
  rapidjson::Document logout_document;
  std::string logout_message;

  logout_resp = Logout(logout_url, auth_token);
  if (logout_resp.code == 200)
  {
    logout_document.Parse(logout_resp.data.c_str());

    if (logout_document.HasMember("message"))
    {
      logout_message = logout_document["message"].GetString();
    }
    std::cout << "Log out: " << logout_message << std::endl;
  }

}