#include "Factory.h"

Tool* Factory(std::string tool) {
Tool* ret=0;

// if (tool=="Type") tool=new Type;
if (tool=="DummyTool") ret=new DummyTool;
if (tool=="MonitorReceive") ret=new MonitorReceive;
if (tool=="LAPPDConfig") ret=new LAPPDConfig;
if (tool=="SlowControlConfig") ret=new SlowControlConfig;
  if (tool=="LAPPDDataReceive") ret=new LAPPDDataReceive;
return ret;
}
