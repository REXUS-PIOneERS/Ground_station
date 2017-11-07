#include <iostream>
#include <utility>
#include "cmd.h"
#include "shared_memory.h"

namespace gnd
{
  void reset(const func_params_t& params)
  {
    std::cout << "RESET called" << std::endl;
    showParams(params);
  }
  
  void shutdown(const func_params_t& params)
  {
    std::cout << "SHUTDOWN called" << std::endl;
    showParams(params);
  }
  
  void invalid(const func_params_t& params)
  {
    std::cout << "Invalid command!" << std::endl;
  }
  
  void showParams(const func_params_t& params)
  {
    for(auto it = params.begin(); it != params.end(); ++it)
      std::cout << *it << std::endl;
  }

  
  std::map<std::string, func_t> command_map =
    {
      {std::make_pair("reset", reset)},
      {std::make_pair("shutdown", shutdown)},
      {std::make_pair("invalid", invalid)},
    };

}
