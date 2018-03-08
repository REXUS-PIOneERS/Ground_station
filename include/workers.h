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
      /**
	 keyboard monitor
       */
      static void* kb_monitor(void* arg);
      /**
	 packet retriever
       */
      static void* packet_retriever(void* arg);

      static rfcom::Transceiver* trPtr;
      static std::ofstream bad_packet_fs; //bad packet log file stream
      static std::ofstream data_fs;  //data file log stream
      
    private:
      /**
	 Write new data log entry to data_fs
	 @params
	 id_pi_num: Masked pi number.
	 id_type: Masked data type.
	 index: packet index
	 data: Data. the length of data is determined by id_type
	 @return
       */
      static inline void _new_data_entry(rfcom::byte1_t id_pi_num, rfcom::byte1_t id_type, rfcom::byte2_t index, const rfcom::byte1_t* data);

      /**
	 Write new bad packet entry to bad_packet_fs
	 @params
	 p: packet
	 error_code: Error_code returned by tryPopUnpack()
	 @return
       */
      static inline void _new_bad_packet_entry(const rfcom::Packet& p, int err_code);

      /**
	 Output array
	 @params
	 pos: head of array
	 len: array length
	 os: output stream
	 @return
       */
      static inline void _array_out(const rfcom::byte1_t* pos, size_t len, std::ostream& os);

      static inline void _acc_gyr_out_1(const rfcom::byte1_t* pos, std::ostream& os);
      static inline void _mag_time_out_1(const rfcom::byte1_t* pos, std::ostream& os);


      /**
	 Newly added
      */
      static inline void _message_out(const rfcom::byte1_t* pos, size_t len, std::ostream& os);

      /**
	 construct a FSM which is used to control line wrap when receiving mesages.
       */
      static void constructWrapController();

      static fsm::FSM<char, bool, rfcom::byte1_t> _line_wrap_controller;
    };
  
}

#endif
