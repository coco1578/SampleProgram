#pragma once

#include <time.h>
#include <atomic>
#include <thread>

#include <Windows.h>

/* Milestone SDK Header */
#include <Toolkits/ToolkitInterface.h>
#include <Toolkits/ToolkitFactoryProvider.h>
#include <Tools/ServerCommandServiceClient.h>

using namespace NmToolkit;
using namespace NmServerCommandService;

class Milestone
{
public:
#pragma region Constructor & Destructor
  Milestone();
  virtual ~Milestone();
#pragma endregion Constructor & Destructor

public:
#pragma region Interface
  void Init();
  void Stop();
  void Run();
#pragma endregion Interface

#pragma region class method
private:
  void init();
  void stop();
  void run();
  bool login(ImServerCommandServiceClient* client, utf8_char_t** token_ptr, std::size_t token_length, NmServerCommandService::utc_time_t* registration_time, NmServerCommandService::time_span_t* time_to_live);
#pragma endregion class method

#pragma region class member variable
private:
  HMODULE module_;

  CreateInstanceFuncPtr_t create_instance_func_ptr_;
  DeleteInstanceFuncPtr_t delete_instance_func_ptr_;

  ImData* data_;
  ImToolkit* toolkit_;
  ImSourceToolkit* source_toolkit_;
  ImLiveSourceToolkit* live_source_toolkit_;
  ImLiveSourceToolkit::get_live_status_t live_data_result_;
  ImToolkitFactory* factory_;
  CmToolkitFactoryProvider provider_;

  utf8_string_t camera_guid_;
  utf8_string_t token_;

  utf8_string_t vms_server_;
  short vms_server_port_;
  utf8_string_t vms_recorder_uri_;
  utf8_string_t username_;
  utf8_string_t password_;
  AuthenticationMethod_t auth_method_;
  XProtectProductFamily_t product_;
  ImServerCommandServiceClient* client_;
  NmServerCommandService::utc_time_t registration_time_;
  NmServerCommandService::time_span_t time_to_live_;

  std::atomic<bool> alive_;
  std::thread thread_;
#pragma endregion class member variable
};