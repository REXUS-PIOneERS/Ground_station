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
	
	else if(*line_seg.begin() == '#')
	  continue;
	
	else if(line_seg == "PORT")
	  {
	    iss >> line_seg;
	    config.port = line_seg;
	  }

	else if(line_seg == "BAUD")
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
	
	else if(line_seg == "RAW_LOG")
	  {
	    iss >> line_seg;
	    config.raw_log = line_seg;
	  }

	else if(line_seg == "PACKET_LOG")
	  {
	    iss >> line_seg;
	    config.packet_log = line_seg;
	  }

	else if(line_seg == "BAD_PACKET_LOG")
	  {
	    iss >> line_seg;
	    config.bad_packet_log = line_seg;
	  }
	
	else if(line_seg == "DATA_LOG")
	  {
	    iss >> line_seg;
	    config.data_log = line_seg;
	  }
      }
    return 0;
  }

}

