#ifndef _WORKERS
#define _WORKERS

#include <map>
#include <string>
#include <fstream>

namespace gnd
{  
  class Workers
    {
    public:
      static void* kb_monitor(void* arg);
      static void* packet_retriever(void* arg);

      static rfcom::Transceiver* trPtr;
      static std::ofstream bad_packet_fs;
      static std::ofstream data_fs;
      
    private:
      static inline void _new_data_entry(rfcom::byte1_t id_pi_num, rfcom::byte1_t id_type, rfcom::byte2_t index, const rfcom::byte1_t* data);
      static inline void _new_bad_packet_entry(const rfcom::Packet& p, int error_code);
      static inline void _array_out(const rfcom::byte1_t* pos, size_t len, std::ostream& os);
      static bool _prev_is_mesg;
    };
  
}

#endif
