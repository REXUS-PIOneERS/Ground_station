#ifndef _CORE
#define _CORE

#include <map>
#include <string>
#include <array>
#include <functional>
#include "radiocom.h"

namespace gnd
{
  class CMD
  {
  public:
    typedef std::array<std::string, 4> func_params_t;
    typedef std::function<void(const func_params_t&)> func_t;

    static void reboot(const func_params_t&);
    static void shutdown(const func_params_t&);
    static void test(const func_params_t&);
    static void mode(const func_params_t&);

    static void invalid(const func_params_t&);


    
    static void showParams(const func_params_t&);
  
    static std::map<std::string, func_t> command_map;  //A look up table of commands

    static rfcom::Transceiver* trPtr;

  private:
    static bool shutdown_called;
    
  };
}

#endif

