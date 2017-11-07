#include <map>
#include <functional>
#include <iostream>
#include "radiocom.h"
#include "shared_memory.h"
#include "workers.h"
#include "cmd.h"
#include "config_parser.h"

using namespace std;
using namespace rfcom;
using namespace gnd;

#define CHECK_FATAL_ERROR(COND, MESSAGE) if((COND)){std::cerr << (MESSAGE) << std::endl;return 0;}

void showStartupInfo()
{
  cout << "Groundstation shell ready." << endl;
  cout << "Supported commands: reset, shutdown, test, status, motor, burnwire" << endl;
  cout << "Type 'help' for details." << endl;
}

int main(int argc, char** argv)
{
  //Check argv
  CHECK_FATAL_ERROR(argc < 2, "A config file is needed!");
  
  int error_code = 0;
  
  //Read in config file
  GndConfig* g_config = new GndConfig;
  error_code = parseConfig(argv[1], *g_config);
  CHECK_FATAL_ERROR(error_code == -1, "Config file " + string(argv[1]) + " does not exist!");
  CHECK_FATAL_ERROR(error_code == -2, "Cannot open config file: " + string(argv[1]) + "!");

  
  //Initialize transceiver
  Transceiver t;
  error_code = t.initPort(g_config->port, g_config->baud, g_config->p_log);
  CHECK_FATAL_ERROR(error_code == -1, "Cannot open serial port: " + g_config->port + "!");
  CHECK_FATAL_ERROR(error_code == -2, "Cannot open log file: " + g_config->p_log + "!");
  
  error_code = t.startListener();
  CHECK_FATAL_ERROR(error_code < 0, "Cannot start listener thread!");
  
  //Initialize shared memory
  M_KM_Shared m_km_mem;
  //To be implmented

  //Initialize threads
  pthread_t kb_monitor_t, file_writer_t;
  error_code = pthread_create(&kb_monitor_t, NULL, Workers::kb_monitor, &m_km_mem);
  //error_code |= pthread_create(&file_writer_t, NULL, Workers::file_writer, &t);
  //To be implemented
  CHECK_FATAL_ERROR(error_code != 0, "One of the crutial threads failed to initialize!");
  

  delete g_config;
  //Initialization completed
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


  
  t.stopListener();
  t.termPort();

  return 0;
}
