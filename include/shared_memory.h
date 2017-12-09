#ifndef _SHARED_MEMORY
#define _SHARED_MEMORY

#include <pthread.h>
#include <vector>
#include <string>
#include <functional>
#include "radiocom.h"


namespace gnd
{  
  typedef std::array<std::string, 4> func_params_t;
  typedef std::function<void(const func_params_t&)> func_t;

  //Shared memory base
  struct Shared_Base
  {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    volatile bool exit_flag;
    
    Shared_Base()
    : lock(PTHREAD_MUTEX_INITIALIZER), cond(PTHREAD_COND_INITIALIZER), exit_flag(false){}
    virtual ~Shared_Base()
    {
      pthread_mutex_destroy(&lock);
      pthread_cond_destroy(&cond);
    }
  };
  
  //Shared between threads: main, keyboard monitor
  struct M_KM_Shared : Shared_Base
  {
    //Command line related function and parameters
    func_t action;
    func_params_t params;
  };
  
  //Shared between threads: main, packet retriever
  struct M_PR_Shared : Shared_Base
  {
  };
  
}

#endif
