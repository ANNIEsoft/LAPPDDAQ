#include "SlowControlConfig.h"

SlowControlConfig::SlowControlConfig():Tool(){}


bool SlowControlConfig::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  std::string ip="";
  std::string port="0";

  if(!m_variables.Get("IP",ip)) ip="127.0.0.1";
  if(!m_variables.Get("port",port)) port="4444";

  SCConfigSend= new zmq::socket_t(*m_data->context, ZMQ_DEALER);

  std::string connection="tcp://"+ip+":"+port;
  SCConfigSend->connect(connection.c_str());
 
  items[0].socket = *SCConfigSend;
  items[0].fd = 0;
  items[0].events = ZMQ_POLLOUT;
  items[0].revents =0;

  // SlowControl_config_Data_class tmp;
  LAPPDMoniterData LAPPDMon;
  // fill config variables for start of run 
  LAPPDMon.SetDefaults();
  LAPPDMon.recieveFlag = 1;
  
  zmq::poll(&items[0], 1, -1);
  
  if ((items [0].revents & ZMQ_POLLOUT)) {
    
    LAPPDMon.Send_Config(SCConfigSend);
    std::cout<<"Sending SlowControl start config variables"<<std::endl;
    LAPPDMon.Print();  
  }


  return true;
}


bool SlowControlConfig::Execute(){

  return true;
}


bool SlowControlConfig::Finalise(){



  delete SCConfigSend;
  SCConfigSend=0;


  return true;
}
