/*************************************************************/
/*    NAME: Muthukumaran Chandrasekaran and Michael Novitzky */
/*    ORGN: MIT                                              */
/*    FILE: MavlinkConverter.cpp                             */
/*    DATE: Oct 12th 2017                                    */
/*************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MavlinkConverter.h"
#include "mavlink.h"

using namespace std;

//---------------------------------------------------------
// Constructor

MavlinkConverter::MavlinkConverter()
{
}

//---------------------------------------------------------
// Destructor

MavlinkConverter::~MavlinkConverter()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool MavlinkConverter::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "FOO") 
       cout << "great!";
     else if(key == "TARGET_SYSTEM") { // check for incoming target system id
       uint8_t system_id = 0;
       uint8_t component_id = 0;
       mavlink_message_t msg;
       uint32_t time_boot_ms = 0;
       uint8_t target_system = 0;
       uint8_t target_component = 0;
       uint8_t coordinate_frame = 5;
       uint16_t type_mask = 0;
       int32_t lat_int = 0;
       int32_t lon_int = 0;
       float alt = 0;
       float vx = 0;
       float vy = 0;
       float vz = 0;
       float afx = 0;
       float afy = 0;
       float afz = 0;
       float yaw = 0;
       float yaw_rate = 0;

       uint16_t length = mavlink_msg_set_position_target_global_int_pack(system_id,component_id,&msg,time_boot_ms,target_system,target_component,coordinate_frame,type_mask,lat_int, lon_int, alt, vx,  vy,  vz,  afx,  afy,  afz,  yaw,  yaw_rate);
       // mavlink_set_position_target_global_int_t sp;
       // sp.target_system = 0;
       char buf[300];

       unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buf, &msg);

       //send binary format to MOOSDB
       //std::vector<unsigned char> X(1000);
       Notify("MAVLINK_MESSAGE",(void*)buf,len);

       //debug of mavlink_message_t for confirmation
       uint8_t test_frame = mavlink_msg_set_position_target_global_int_get_coordinate_frame(&msg);
       Notify("VERIFY_FRAME",test_frame);
     }

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool MavlinkConverter::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool MavlinkConverter::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool MavlinkConverter::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "FOO") {
      handled = true;
    }
    else if(param == "BAR") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void MavlinkConverter::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("TARGET_SYSTEM",0);
  Register("TARGET_COMPONENT",0);
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MavlinkConverter::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}




