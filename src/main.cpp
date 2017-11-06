#include <map>
#include <functional>
#include <iostream>
#include "../../Radio_comm/include/radiocom.h"
#include "../include/shared_memory.h"
#include "../include/workers.h"
#include "../include/core.h"

using namespace std;
using namespace rfcom;
using namespace gnd;

void showStartupInfo()
{
  cout << "Groundstation shell ready." << endl;
  cout << "Supported commands: reset, shutdown, test, status, motor, burnwire" << endl;
  cout << "Type 'help' for details." << endl;
}

int main(int argc, char** argv)
{
  //Initialize shared memory
  M_KM_Shared m_km_mem;
  //To be implmented

  //Initialize threads
  pthread_t kb_monitor_t;
  pthread_create(&kb_monitor_t, NULL, Workers::kb_monitor, &m_km_mem);
  //To be implemented
  
  showStartupInfo();

  while(true)
    {
      cout << ">>>" << flush;
      pthread_mutex_lock(&m_km_mem.lock);
      pthread_cond_wait(&m_km_mem.cond, &m_km_mem.lock);
      if(m_km_mem.exit_flag)
	{
	  pthread_mutex_unlock(&m_km_mem.lock);
	  break;
	}
      m_km_mem.action(m_km_mem.params);
      pthread_mutex_unlock(&m_km_mem.lock);
    }

  pthread_join(kb_monitor_t, NULL);
  

  return 0;
}
