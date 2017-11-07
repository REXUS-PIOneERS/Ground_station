#include "config_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <termios.h>
#include <sys/stat.h>

namespace gnd
{
  int parseConfig(const std::string& name, GndConfig& config)
  {
    std::ifstream ifs(name);
    if(ifs.fail())
      return -1;
    if(!ifs.is_open())
      return -2;

    std::string line;
    std::string line_seg;
    int baudrate;
    std::istringstream iss;
    while(std::getline(ifs, line))
      {
	iss.clear();
	line_seg.clear();
	
	iss.str(line);
	iss >> line_seg;
	
	//An empty line
	if(line_seg.empty())
	  continue;
	
	//A comment
	if(*line_seg.begin() == '#')
	  continue;
	
	//PORT
	if(line_seg == "PORT")
	  {
	    iss >> line_seg;
	    config.port = line_seg;
	  }

	//BAUD
	if(line_seg == "BAUD")
	  {
	    iss >> baudrate;
	    switch(baudrate)
	      {
	      case 19200:
		config.baud = B19200;
		break;
	      case 57600:
		config.baud = B57600;
		break;
	      case 115200:
		config.baud = B115200;
		break;
	      case 230400:
		config.baud = B230400;
		break;

	      default:
	      case 38400:
		config.baud = B38400;
		break;
	      }
	  }
	
	//P_LOG
	if(line_seg == "P_LOG")
	  {
	    iss >> line_seg;
	    config.p_log = line_seg;
	  }

	//C_LOG
	if(line_seg == "C_LOG")
	  {
	    iss >> line_seg;
	    config.c_log = line_seg;
	  }

	//D_LOG
	if(line_seg == "D_LOG")
	  {
	    iss >> line_seg;
	    config.d_log = line_seg;
	  }

      }
    return 0;
  }

}

