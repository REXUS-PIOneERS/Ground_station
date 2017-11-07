#ifndef _CORE
#define _CORE

#include <map>
#include <string>
#include <array>
#include <functional>

namespace gnd
{
  typedef std::array<std::string, 4> func_params_t;
  typedef std::function<void(const func_params_t&)> func_t;

  void reset(const func_params_t&);
  void shutdown(const func_params_t&);
  void invalid(const func_params_t&);

  void showParams(const func_params_t&);



  /*struct GndConfig
  {
    std::string 
    }*/
  void readConfigFile(const std::string& filename);
  
  extern std::map<std::string, func_t> command_map;
}

#endif

