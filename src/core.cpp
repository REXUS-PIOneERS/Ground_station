#include <iostream>
#include <utility>
#include "../include/core.h"

namespace gnd
{
  void reset(const func_params_t& params)
  {
    std::cout << "RESET called" << std::endl;
    std::cout << "Params: " << params[0] << " " << params[1] << " " << params[2] << std::endl;
  }
  
  void shutdown(const func_params_t& params)
  {
    std::cout << "SHUTDOWN called" << std::endl;
    std::cout << "Params: " << params[0] << " " << params[1] << " " << params[2] << std::endl;
  }
  
  void invalid(const func_params_t& params)
  {
    std::cout << "Invalid command!" << std::endl;
  }
  
  std::map<std::string, func_t> command_map =
    {
      {std::make_pair("reset", reset)},
      {std::make_pair("shutdown", shutdown)},
      {std::make_pair("invalid", invalid)},
    };
      /*{"test", test},
  {"status", status},
  {},
  {},
  {},
  {}*/
}
