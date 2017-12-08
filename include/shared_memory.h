#ifndef _SHARED_MEMORY
#define _SHARED_MEMORY

#include <pthread.h>
#include <vector>
#include <string>
#include <functional>


namespace gnd
{  
  typedef std::array<std::string, 4> func_params_t;
  typedef std::function<void(const func_params_t&)> func_t;

  //Shared memory base
  struct Shared_Base
  {
    pthread_mutex_t lock;
    pthread_cond_t cond;

    Shared_Base()
      : lock(PTHREAD_MUTEX_INITIALIZER), cond(PTHREAD_COND_INITIALIZER){}
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
    
    volatile bool exit_flag;
    M_KM_Shared()
    : exit_flag(false){}
  };
  
  //Shared between threads: main, packet retriever
  struct M_PR_Shared
  {
    volatile bool exit_flag;
    M_PR_Shared()
    : exit_flag(false){}
  };
  
}

#endif
