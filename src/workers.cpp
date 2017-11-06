#include <iostream>
#include <string>
#include <algorithm>
#include <ctype.h>
#include <sstream>
#include "../include/workers.h"
#include "../include/shared_memory.h"
#include "../include/core.h"

namespace gnd
{
  void* Workers::kb_monitor(void* arg)
  {        
    M_KM_Shared* mem_ptr = static_cast<M_KM_Shared*>(arg);
    std::string input, command;
    size_t params_count;
    decltype(command_map)::iterator m_it;
    std::istringstream iss;
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
	if((m_it = command_map.find(command)) == command_map.end())
	  m_it = command_map.find("invalid");
	
	mem_ptr->action = m_it->second;
	pthread_cond_signal(&mem_ptr->cond);
	pthread_mutex_unlock(&mem_ptr->lock);
      }    
    pthread_exit(NULL);
  }
}
