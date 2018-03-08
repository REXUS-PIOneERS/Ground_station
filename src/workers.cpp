#include <iostream>
#include <string>
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
	command.clear();
	iss >> command;
	//blank line
	if(command == "")
	  {
	    iss.clear();
	    std::cout << ">>> " << std::flush;
	    continue;
	  }
	
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
	
	//command selected
	mem_ptr->action = m_it->second;
	pthread_cond_signal(&mem_ptr->cond);
	pthread_mutex_unlock(&mem_ptr->lock);
      }    
    pthread_exit(NULL);
  }

#define GET_INDEX_H(X) (X & 0xff00)
#define GET_INDEX_L(X) (X & 0x00ff)
  
#define GET_DSM(X)    (X & 0xc0)
#define GET_PI_NUM(X) (X & 0x30)
#define GET_TYPE(X)   (X & 0x0f)

#define DSM_DATA (0x00)
#define DSM_STAT (0x40)
#define DSM_MESG (0x80)

#define PI_NUM_1 (0x10)
#define PI_NUM_2 (0x20)

#define DATA_TYPE_AG (0x00)
#define DATA_TYPE_MT (0x01)
#define DATA_TYPE_MTI (0x02)

#define STAT_TYPE_GEN (0x00)
#define STAT_TYPE_ENC (0x01)

#define MESG_TYPE_GEN   (0x00)
#define MESG_TYPE_ERR   (0x01)
#define MESG_TYPE_FATAL (0x02)
  
  void* Workers::packet_retriever(void* arg)
  {
    M_PR_Shared* mem_ptr = static_cast<M_PR_Shared*>(arg);
    rfcom::Packet p;
    rfcom::byte1_t id;
    rfcom::byte1_t id_dsm;
    rfcom::byte1_t id_pi_num;
    rfcom::byte1_t id_type;
    rfcom::byte2_t index;
    rfcom::byte1_t data[16];
    int err_code = 0;
    rfcom::byte1_t line_wrap_cond = 0x00;
    
    //Construct an FSM to deal with line wraps
    constructWrapController();
       
    pthread_mutex_lock(&mem_ptr->lock);
    //Even when exit flag is set, will still work until PDU stream is clean.
    while((err_code = trPtr->tryPopUnpack(id, index, data)) != -1 || !mem_ptr->exit_flag)
      {
	pthread_mutex_unlock(&mem_ptr->lock);
	//PDU stream empty
	if(err_code == -1)
	  {
	    usleep(1000);
	    pthread_mutex_lock(&mem_ptr->lock);
	    continue;
	  }
	
	//Bad packet
	else if(err_code < 0)
	  {
	    trPtr->extractNext(p);
	    _new_bad_packet_entry(p, err_code);
	    continue;
	  }

	//Good packet
	else
	  {
	    id_dsm = GET_DSM(id);
	    id_pi_num = GET_PI_NUM(id);
	    id_type = GET_TYPE(id);
	    
	    switch(id_dsm)
	      {
	      case DSM_MESG:
		_message_out(data, 16, std::cout);
		break;
		
	      case DSM_DATA:
		_new_data_entry(id_pi_num, id_type, index, data);
		break;

	      case DSM_STAT:
		data_fs << "Status packet?" << std::endl;
		break;
		
	      default:
		data_fs << "Unknown packet?" << std::endl;
		break;
	      }

	    //Line wrap control
	    line_wrap_cond = (id_dsm == DSM_MESG) ? 0x02 : 0x00;
	    line_wrap_cond |= (GET_INDEX_L(index)) ? 0x01 : 0x00;
	    _line_wrap_controller.transit(line_wrap_cond);
	    if(_line_wrap_controller.getCurrOutput())
	      std::cout << "\n>>> " << std::flush;
	  }
	//Lock it up for the next round
	pthread_mutex_lock(&mem_ptr->lock);
      }
    pthread_mutex_unlock(&mem_ptr->lock);
    pthread_exit(NULL);
  }

  void Workers::_new_bad_packet_entry(const rfcom::Packet& p, int err_code)
  {
    //COBS decode error
    if(err_code == -2)
      {
	bad_packet_fs << "COBS: ";
	rfcom::packetOut(p, bad_packet_fs);
	bad_packet_fs << std::endl;
      }
    //CRC mismatch
    else
      {
	bad_packet_fs << "CRC : ";
	rfcom::packetOut(p, bad_packet_fs);
	bad_packet_fs << std::endl;
      }
  }
  
  void Workers::_new_data_entry(rfcom::byte1_t id_pi_num, rfcom::byte1_t id_type, rfcom::byte2_t index, const rfcom::byte1_t* data)
  {
    //determine which pi does this packet come from
    switch(id_pi_num)
      {
      case PI_NUM_1:
	data_fs << "PI1: ";
	break;
      case PI_NUM_2:
	data_fs << "PI2: ";
	break;
      default:
	data_fs << "PI*: ";
	break;
      }

    //determine data type
    switch (id_type)
      {
	// acc/gyr
      case DATA_TYPE_AG:
	data_fs << "acc/gyr:\t[" << index << "]" << "\t";
	//_array_out(data, 12, data_fs);
  _acc_gyr_out_1(data, data_fs);
	data_fs << std::endl;
	break;
	// mag/time
      case DATA_TYPE_MT:
	data_fs << "mag/time:\t[" << index << "]" << "\t";
	//_array_out(data, 10, data_fs);
  _mag_time_out_1(data, data_fs);
	data_fs << std::endl;
	break;
	//mag/time/imp
      case DATA_TYPE_MTI:
	data_fs << "mag/imp/time:\t[" << index << "]" << "\t";
	_array_out(data, 12, data_fs);
	data_fs << std::endl;
	break;
      default:
	data_fs << "unknown:\t[" << index << "]" << "\t";
	_array_out(data, 16, data_fs);
	data_fs << std::endl;
	break;	
      }
  }
  
  void Workers::_acc_gyr_out_1(const rfcom::byte1_t* pos, std::ostream& os) {
    int16_t combined[6];
    double acc_values[3];
    double gyr_values[3];
    for (int i = 0; i < 12; i += 2)
        combined[i/2] = (int16_t)(pos[i] & (pos[i+1] << 8))
    
    for (int i = 0; i < 3; i++)
      acc_values[i] = ((double)combined[i])/(8192)

    for (int i = 0; i < 3; i++)
      gyr_values[i] = ((double)combined[i+3] * 125)/(1024)

    os << "acc_x-" << acc_values[0] << " " <<
          "acc_y-" << acc_values[1] << " " <<
          "acc_z-" << acc_values[2] << " " <<
          "gyr_x-" << gyr_values[0] << " " <<
          "gyr_y-" << gyr_values[1] << " " <<
          "gyr_z-" << gyr_values[2];
    
    return;
  }

  void Workers::_mag_time_out_1(const rfcom::byte1_t* pos, std::ostream& os) {
    double mag_values[3];
    int32_t time_stamp;
    for (int i = 0; i < 6; i += 2) {
      mag_values[i/2] = (int16_t)(pos[i] & (pos[i+1] << 8));
      mag_values /= 8192;    
    }

    time_stamp = (int32_t)(pos[7] & (pos[8] << 8) & (pos[9] << 16) & (pos[10] << 32));

    os << "mag_x-" << mag_values[0] << " " <<
          "mag_y-" << mag_values[1] << " " <<
          "mag_z-" << mag_values[2] << " " <<
          "time-"  << time_stamp;
    return;
  }

  void Workers::_array_out(const rfcom::byte1_t* pos, size_t len, std::ostream& os)
  {
    os << std::hex;
    auto os_it = std::ostream_iterator<int>(os, " ");
    std::copy(pos, pos + len, os_it);
  }

  void Workers::_message_out(const rfcom::byte1_t* pos, size_t len, std::ostream& os)
  {
    auto os_it = std::ostream_iterator<char>(os, "");
    std::copy(pos, pos + len, os_it);
    os << std::flush;
  }

  
  void Workers::constructWrapController()
  {
    /*Output boolean type, indicate whether a line wrap is needed
      Transition type byte1_t
      0x00 --- is not a message packet, lower index zero.
      0x01 --- is not a message packet, lower index non-zero.
      0x02 --- is a message packet, lower index zero.
      0x03 --- is a message packet, lower index non-zero.

      Generally, this FSM will tell packet retriever to wrap line when 
      1. If this is a message packet, lower byte of index = 0.
      2. If this is not a message packet, but previous packet is a message packet.
    */
    _line_wrap_controller.setState('a', false);
    _line_wrap_controller.setState('b', true);
    _line_wrap_controller.setState('c', true);
    _line_wrap_controller.setState('d', false);
    _line_wrap_controller.setState('0', false);

    _line_wrap_controller.setTrans('a', 0x00, 'c');
    _line_wrap_controller.setTrans('a', 0x01, 'c');
    _line_wrap_controller.setTrans('a', 0x02, 'b');
    _line_wrap_controller.setTrans('a', 0x03, 'a');
    
    _line_wrap_controller.setTrans('b', 0x00, 'd');
    _line_wrap_controller.setTrans('b', 0x01, 'd');
    _line_wrap_controller.setTrans('b', 0x02, 'b');
    _line_wrap_controller.setTrans('b', 0x03, 'a');
    
    _line_wrap_controller.setTrans('c', 0x00, 'd');
    _line_wrap_controller.setTrans('c', 0x01, 'd');
    _line_wrap_controller.setTrans('c', 0x02, 'b');
    _line_wrap_controller.setTrans('c', 0x03, 'a');
    
    _line_wrap_controller.setTrans('d', 0x00, 'd');
    _line_wrap_controller.setTrans('d', 0x01, 'd');
    _line_wrap_controller.setTrans('d', 0x02, 'b');
    _line_wrap_controller.setTrans('d', 0x03, 'a');
    
    _line_wrap_controller.setTrans('0', 0x00, 'd');
    _line_wrap_controller.setTrans('0', 0x01, 'd');
    _line_wrap_controller.setTrans('0', 0x02, 'b');
    _line_wrap_controller.setTrans('0', 0x03, 'a');
  }
  
  rfcom::Transceiver* Workers::trPtr = NULL;
  std::ofstream Workers::bad_packet_fs;
  std::ofstream Workers::data_fs;
  fsm::FSM<char, bool, rfcom::byte1_t> Workers::_line_wrap_controller;
}
