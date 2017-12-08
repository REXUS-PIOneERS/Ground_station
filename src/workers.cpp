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

  void* Workers::packet_retriever(void* arg)
  {
    rfcom::Transceiver* t_ptr = static_cast<rfcom::Transceiver*>(arg);
    rfcom::Packet p;
    rfcom::byte1_t id;
    rfcom::byte2_t index;
    rfcom::byte1_t data[16];
    int err_code = 0;
    
    while(true)
      {
	//Wait for packet
	while((err_code = t_ptr->tryPopUnpack(id, index, data)) < 0)
	  {
	    if(err_code == -2)
	      std::cout << "COBS error" << std::endl;
	    if(err_code == -3)
	      std::cout << "CRC error" << std::endl;

	    t_ptr->extractNext(p);
	    
	    //rfcom::packetOut(p, std::cout);
	    
	    usleep(1000);
	    continue;
	  }
	
	//rfcom::packetOut(p, std::cout);

	//A message
	if(id == 0x90)
	  {
	    for(int count = 0; count < 16; ++count)
	      std::cout << data[count];
	    std::cout << std::endl;
	  }
      }
  }
}
