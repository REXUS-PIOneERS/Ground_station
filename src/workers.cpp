#include <iostream>
#include <string>
#include <algorithm>
#include <ctype.h>
#include <sstream>
#include "radiocom.h"
#include "workers.h"
#include "shared_memory.h"
#include "cmd.h"


namespace gnd
{
  void* Workers::kb_monitor(void* arg)
  {        
    M_KM_Shared* mem_ptr = static_cast<M_KM_Shared*>(arg);
    std::string input, command;
    size_t params_count;
    decltype(CMD::command_map)::iterator m_it;
    std::istringstream iss;

    //cin will automatically block.
    while(std::getline(std::cin, input))
      {
	iss.str(input);
	//Acquired command from input
	iss >> command;
	
	pthread_mutex_lock(&mem_ptr->lock);
	//Acquire params from input
	mem_ptr->params.fill("");
	for(params_count = 0; params_count < mem_ptr->params.size(); ++params_count)
	  if(!(iss >> mem_ptr->params[params_count]))
	    break;
	iss.clear();
	
	//Quit the entire program
	if(command == "exit")
	  {
	    mem_ptr->exit_flag = true;
	    pthread_cond_signal(&mem_ptr->cond);
	    pthread_mutex_unlock(&mem_ptr->lock);
	    break;
	  }

	//command invalid
	if((m_it = CMD::command_map.find(command)) == CMD::command_map.end())
	  m_it = CMD::command_map.find("invalid");
	
	//valid commands
	mem_ptr->action = m_it->second;
	pthread_cond_signal(&mem_ptr->cond);
	pthread_mutex_unlock(&mem_ptr->lock);
      }    
    pthread_exit(NULL);
  }

  void* Workers::file_writer(void* arg)
  {
    rfcom::Transceiver* t_ptr = static_cast<rfcom::Transceiver*>(arg);
    rfcom::Packet p;

    while(true)
      {
	//Wait for packet
	while(!t_ptr->extractNext(p)){usleep(100000);}
	
	rfcom::packetOut(p, std::cout);
	std::cout << std::endl;
      }
  }
}
