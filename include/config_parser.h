#ifndef _CONFIG_PARSER
#define _CONFIG_PARSER

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <termios.h>
#include <map>

namespace gnd
{
  struct GndConfig
  {
    std::string port;
    std::string p_log;
    std::string c_log;
    std::string d_log;
    
    speed_t baud;
  };

  /**
     Parse config file into GndConfig object
     @params
     config: configuration file name
     @return
     0: Successfully parsed
     -1: config file not exist
     -2: cannot open config file
   */
  int parseConfig(const std::string& name, GndConfig& config);
}





#endif
