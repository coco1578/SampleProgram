#include "Milestone.h"

#pragma region Constructor & Destructor
Milestone::Milestone() :
  data_(nullptr),
  toolkit_(nullptr),
  source_toolkit_(nullptr),
  live_source_toolkit_(nullptr),
  factory_(nullptr),
  camera_guid_(""),
  vms_server_(""),
  vms_server_port_(0),
  vms_recorder_uri_(""),
  username_(""),
  password_(""),
  auth_method_(WindowsAuthentication),
  product_(XProtectCorporateFamily),
  client_(nullptr),
  registration_time_(0),
  time_to_live_(0),
  alive_(false)
{
  module_ = LoadLibrary(L"ServerCommandServiceClient");
  if (module_ == NULL)
    return;

  create_instance_func_ptr_ = reinterpret_cast<CreateInstanceFuncPtr_t>(GetProcAddress(module_, "CreateInstance"));
  delete_instance_func_ptr_ = reinterpret_cast<DeleteInstanceFuncPtr_t>(GetProcAddress(module_, "DeleteInstance"));

  if (create_instance_func_ptr_ == NULL)
    return;
  if (delete_instance_func_ptr_ == NULL)
    return;
}

Milestone::~Milestone()
{

}
#pragma endregion Constructor & Destructor

#pragma region Interface
void Milestone::Init()
{
  init();
}

void Milestone::Stop()
{
  stop();
}

void Milestone::Run()
{
  run();
}
#pragma endregion Interface

#pragma region class method
void Milestone::init()
{
  try
  {
    client_ = create_instance_func_ptr_();
    client_->SetServerHostName(vms_server_.c_str(), vms_server_.length());
    client_->SetServerPort(vms_server_port_);
    client_->SetUserName(username_.c_str(), username_.length());
    client_->SetPassword(password_.c_str(), password_.length());
    client_->SetAuthenticationMethod(auth_method_);
    client_->SetServerProduct(product_);

    utf8_char_t* token_ptr = NULL;
    std::size_t token_length = 0;

    if (!login(client_, &token_ptr, &token_length, &registration_time_, &time_to_live_))
    {
      // Login failed
    }

    const std::string token(token_ptr, token_length);
    token_ =
      "<authorization method='token'>"
      "   <token update_key='token_key'>" + token + "</token>"
      "</authorization>";

    if ((factory_ = provider_.CreateInstance()) == nullptr)
    {
      // return false;
    }

    utf8_string_t config = "<?xml version='1.0' encoding='utf-8'?>"
      "        <toolkit type='source'>"
      "          <provider>is</provider>"
      "          <config>"
      "            <server_uri>" + vms_recorder_uri_ + "</server_uri>"
      "            <device_id>" + camera_guid_ + "</device_id>"
      "            <media_type>VIDEO</media_type>" + token_ +
      "          </config>"
      "        </toolkit>";

    toolkit_ = factory_->CreateInstance(config);
    source_toolkit_ = dynamic_cast<ImSourceToolkit*>(toolkit_);

    if (source_toolkit_ == nullptr)
    {
      // error
    }
    else
    {
      // Trying to connect camera
      source_toolkit_->UpdateConfiguration("compression_rate", "100");
      source_toolkit_->Connect();
      live_source_toolkit_ = dynamic_cast<ImLiveSourceToolkit*>(source_toolkit_);

      if (live_source_toolkit_ == nullptr)
      {
        // error
      }
      else
      {
        live_source_toolkit_->StartLiveStream();

        // Start to receive packets...
        // mAlive = 0x1; -> thread control
        // mLiveThread = (HANDLE)_beginthreadex(NULL, 0, ImgReceiver::RecvThread, this, 0, NULL);
        alive_ = true;
        std::thread thread(std::bind(&Milestone::run, this));
        thread_ = std::move(thread);
      }
    }
  }
  catch (const ImToolkitError& error)
  {
    return;
  }
}

void Milestone::stop()
{
  alive_ = false;
  if (thread_.joinable())
    thread_.join();
}

void Milestone::run()
{

}
#pragma endregion class method