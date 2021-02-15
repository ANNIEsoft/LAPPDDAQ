#include "LAPPDDataReceive.h"

LAPPDDataReceive::LAPPDDataReceive():Tool(){}


bool LAPPDDataReceive::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  std::string ip="";
  std::string port="0";

  if(!m_variables.Get("IP",ip)) ip="127.0.0.1";
  if(!m_variables.Get("port",port)) port="8888";

  DataReceive = new zmq::socket_t(*m_data->context, ZMQ_DEALER);

  std::string connection="tcp://"+ip+":"+port;
  DataReceive->connect(connection.c_str());
 
  items[0].socket = *DataReceive;
  items[0].fd = 0;
  items[0].events = ZMQ_POLLIN;
  items[0].revents =0;

  return true;
}


bool LAPPDDataReceive::Execute(){

  zmq::poll(&items[0], 1, 100);
  
  if ((items [0].revents & ZMQ_POLLIN)) {
    
    m_data->psec.Receive(DataReceive);
    std::cout<<"Received LAPPD Data"<<std::endl;
    m_data->psec.Print();
  
  }


  return true;
}


bool LAPPDDataReceive::Finalise(){

  delete DataReceive;
  DataReceive=0;

  return true;
}
