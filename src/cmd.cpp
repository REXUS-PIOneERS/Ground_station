#include <string>
#include <iostream>
#include <utility>
#include "cmd.h"
#include "shared_memory.h"
#include "packet.h"

#define CMD_ID 0xc0
#define SENT_FEEDBACK(X)   std::cout << "Command -- " << (X) << ": sent!" << std::endl
#define IGNORE_FEEDBACK(X)   std::cout << "Command -- " << (X) << ": igored!" << std::endl
#define UNSENT_FEEDBACK(X)   std::cout << "Command -- " << (X) << ": Unable to send, failed to write to ttyS!" << std::endl
#define INVALID_PARAM_FEEDBACK(X, Y)  std::cout << "Parameter -- " << (Y) << ": Invalid parameter for command " << (X) << "!" << std::endl

//extern rfcom::Transceiver tr;

namespace gnd
{
 
  void CMD::reboot(const func_params_t& params)
  {
    rfcom::byte1_t cmd_data[2] = {0x01, 0x00};
    if(trPtr->packSend(CMD_ID, cmd_data) == 0)
      SENT_FEEDBACK("reboot");
    else
      SENT_FEEDBACK("reboot");
  }
  void CMD::shutdown(const func_params_t& params)
  {
    //First time call shutdown
    if(!shutdown_called)
      {
	shutdown_called = true;
	std::cout << "Shutdown? re-enter [shutdown] to confirm" << std::endl;
      }
    //second time confirm
    else
      {
	shutdown_called = false;    
	rfcom::byte1_t cmd_data[2] = {0x02, 0x00};
	if(trPtr->packSend(CMD_ID, cmd_data) == 0)
	  SENT_FEEDBACK("shutdown");
	else
	  UNSENT_FEEDBACK("shutdown");
      }
  }
  
  void CMD::mode(const func_params_t& params)
  {
    //mode flight
    if(params[0] == "flight")
    {
      rfcom::byte1_t cmd_data[2] = {0x03, 0x01};
      if(trPtr->packSend(CMD_ID, cmd_data) == 0)
        SENT_FEEDBACK("mode flight");
      else
        UNSENT_FEEDBACK("mode flight");
    }
    else if(params[0] == "test")
    {
	    rfcom::byte1_t cmd_data[2] = {0x03, 0x00};
      if(trPtr->packSend(CMD_ID, cmd_data) == 0)
        SENT_FEEDBACK("mode flight");
      else
        UNSENT_FEEDBACK("mode flight");
    }    


    //Invalid parameter
    else
      INVALID_PARAM_FEEDBACK("mode", params[0]);
  }
  
  void CMD::test(const func_params_t& params)
  {
    //test all
    if(params[0] == "all")
      {
	rfcom::byte1_t cmd_data[2] = {0x04, 0x00};
	if(trPtr->packSend(CMD_ID, cmd_data) == 0)
	  SENT_FEEDBACK("test all");
	else
	  UNSENT_FEEDBACK("test all");
      }
    //test imu
    else if(params[0] == "imu")
      {
	rfcom::byte1_t cmd_data[2] = {0x04, 0x01};
	if(trPtr->packSend(CMD_ID, cmd_data) == 0)
	  SENT_FEEDBACK("test imu");
	else
	  UNSENT_FEEDBACK("test imu");
      }
    //test camera
    else if(params[0] == "camera")
      {
	rfcom::byte1_t cmd_data[2] = {0x04, 0x02};
	if(trPtr->packSend(CMD_ID, cmd_data) == 0)
	  SENT_FEEDBACK("test camera");
	else
	  UNSENT_FEEDBACK("test camera");
      }
    //test imp
    else if(params[0] == "imp")
      {
	rfcom::byte1_t cmd_data[2] = {0x04, 0x03};
	if(trPtr->packSend(CMD_ID, cmd_data) == 0)
	  SENT_FEEDBACK("test imp");
	else
	  UNSENT_FEEDBACK("test imp");
      }


    //Invalid parameter
    else
      INVALID_PARAM_FEEDBACK("test", params[0]);
  }

  void CMD::clean(const func_params_t&params)
  {
    if(params[0] == "all")
    {
      rfcom::byte1_t cmd_data[2] = {0x05, 0x00};
      if(trPtr->packSend(CMD_ID, cmd_data) == 0)
        SENT_FEEDBACK("clean all");
      else
        UNSENT_FEEDBACK("clean all");      
    }
    else if(params[0] == "data")
    {
      rfcom::byte1_t cmd_data[2] = {0x05, 0x01};
      if(trPtr->packSend(CMD_ID, cmd_data) == 0)
        SENT_FEEDBACK("clean data");
      else
        UNSENT_FEEDBACK("clean data");      
    }
    else if(params[0] == "video")
    {
      rfcom::byte1_t cmd_data[2] = {0x05, 0x02};
      if(trPtr->packSend(CMD_ID, cmd_data) == 0)
        SENT_FEEDBACK("clean video");
      else
        UNSENT_FEEDBACK("clean video");      
    }
    else if(params[0] == "logs")
    {
      rfcom::byte1_t cmd_data[2] = {0x05, 0x03};
      if(trPtr->packSend(CMD_ID, cmd_data) == 0)
        SENT_FEEDBACK("clean logs");
      else
        UNSENT_FEEDBACK("clean logs");      
    }
  }

  void CMD::rebuild(const func_params_t& params)
  {
    rfcom::byte1_t cmd_data[2] = {0x06, 0x00};
    if(trPtr->packSend(CMD_ID, cmd_data) == 0)
      SENT_FEEDBACK("rebuild");
    else
      UNSENT_FEEDBACK("rebuild");      
  }

      
  void CMD::invalid(const func_params_t& params)
  {
    std::cout << "Invalid command!" << std::endl;
  }
  
  void CMD::showParams(const func_params_t& params)
  {
    for(auto it = params.begin(); it != params.end(); ++it)
      std::cout << *it << std::endl;
  }

  
  std::map<std::string, func_t> CMD::command_map =
    {
      {std::make_pair("reboot", reboot)},
      {std::make_pair("shutdown", shutdown)},
      {std::make_pair("mode", mode)},
      {std::make_pair("test", test)},
      {std::make_pair("invalid", invalid)},
      {std::make_pair("clean",clean)},
      {std::makepair("rebuild",rebuild)}
    };

  rfcom::Transceiver* CMD::trPtr = NULL;
  bool CMD::shutdown_called = false;
}
