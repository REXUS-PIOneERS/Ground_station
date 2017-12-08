#ifndef _WORKERS
#define _WORKERS

#include <map>
#include <string>

namespace gnd
{  
  class Workers
    {
    public:
      static void* kb_monitor(void* arg);
      static void* packet_retriever(void* arg);
      
    };

}

#endif
