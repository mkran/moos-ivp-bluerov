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
#include "time.h"
#include "sys/time.h"
#include "math.h"

#define SYSTEM_ID 255
#define COMPONENT_ID 0
#define TARGET_SYSTEM 1
#define TARGET_COMPONENT 0

#define COORDINATE_FRAME 8

// 1) For both velocity (vx,vy,vz) and yaw angle: type_mask = 2503; 
// 2) For just yaw angle: type_mask = 2559; 
// 3) For just velocity (vx,vy,vz): type_mask = 455;
#define TYPE_MASK 2503 

// Max rate to receive MOOS updates (0 indicates no rate, get all)
#define DEFAULT_REGISTER_RATE 0.0

// Enable/disable debug code
#define DEBUG 1

using namespace std;

//---------------------------------------------------------
// Constructor

/**
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_boot_ms Timestamp in milliseconds since system boot
 * @param target_system System ID
 * @param target_component Component ID
 * @param coordinate_frame Valid options are: MAV_FRAME_LOCAL_NED = 1, MAV_FRAME_LOCAL_OFFSET_NED = 7, MAV_FRAME_BODY_NED = 8, MAV_FRAME_BODY_OFFSET_NED = 9
 * @param type_mask Bitmask to indicate which dimensions should be ignored by the vehicle: a value of 0b0000000000000000 or 0b0000001000000000 indicates that none of the setpoint dimensions should be ignored. If bit 10 is set the floats afx afy afz should be interpreted as force instead of acceleration. Mapping: bit 1: x, bit 2: y, bit 3: z, bit 4: vx, bit 5: vy, bit 6: vz, bit 7: ax, bit 8: ay, bit 9: az, bit 10: is force setpoint, bit 11: yaw, bit 12: yaw rate
 * @param x X Position in NED frame in meters
 * @param y Y Position in NED frame in meters
 * @param z Z Position in NED frame in meters (note, altitude is negative in NED)
 * @param vx X velocity in NED frame in meter / s
 * @param vy Y velocity in NED frame in meter / s
 * @param vz Z velocity in NED frame in meter / s
 * @param afx X acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
 * @param afy Y acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
 * @param afz Z acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N
 * @param yaw yaw setpoint in rad
 * @param yaw_rate yaw rate setpoint in rad/s
 * @return length of the message in bytes (excluding serial stream start sign)
 */

MavlinkConverter::MavlinkConverter()
{
  // m_mavlink_msg = new mavlink_message_t();
  system_id = 0;
  component_id = 0;
  time_boot_ms = 0;
  target_system = 0;
  target_component = 0;
  coordinate_frame = 0;
  type_mask = 0; 
  x=0.0;
  y=0.0;
  z=0.0;
  vx = 0.0;
  vy = 0.0;
  vz = 0.0;
  afx = 0.0;
  afy = 0.0;
  afz = 0.0;
  yaw = 0.0;
  yaw_rate = 0.0;
  m_desired_speed = 0.0;
  m_desired_heading_deg = 0.0;
  m_desired_heading_rad = 0.0;
}

//---------------------------------------------------------
// Destructor

MavlinkConverter::~MavlinkConverter()
{
}

// ----------------------------------------------------------------------------------
//   Time
// ------------------- ---------------------------------------------------------------
uint64_t get_time_usec(){
  static struct timeval _time_stamp;
  gettimeofday(&_time_stamp, NULL);
  return _time_stamp.tv_sec*1000000 + _time_stamp.tv_usec;
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
    cout<<"Key: "<<key<<endl;
    double mdbl=-1.0;
    if(msg.IsDouble()){
      mdbl=msg.GetDouble();
    }
    string mstr="";
    if(msg.IsString()){
      mstr=msg.GetString();
    }
    
    #if 0 // Keep these around just for template
        string comm  = msg.GetCommunity();
        double dval  = msg.GetDouble();
        string sval  = msg.GetString(); 
        string msrc  = msg.GetSource();
        double mtime = msg.GetTime();
        bool   mdbl  = msg.IsDouble();
        bool   mstr  = msg.IsString();
    #endif
    
    if(key == "DESIRED_SPEED") { // check for incoming target speed 
      if(mdbl>=0){
        m_desired_speed = (float)mdbl;
      }else if(mstr.compare("") != 0){
        m_desired_speed = atof(mstr.c_str());
      }else{
        m_desired_speed = -2.0;
        cout<<"Crappy Speed!"<<endl;
      }
    }

    if(key == "DESIRED_HEADING") {
      if(mdbl>=0){
        m_desired_heading_deg = (float)mdbl;
      }else if(mstr.compare("") != 0){
        m_desired_heading_deg = atof(mstr.c_str());
      }else{
        m_desired_heading_deg = -4.0;
        cout<<"Crappy Heading!"<<endl;
      }
    }

    if(key == "DESIRED_HEADING" || key == "DESIRED_SPEED"){

      m_desired_heading_rad = (float) m_desired_heading_deg * (PI / 180.0);
      vy = (float) m_desired_speed * sin (m_desired_heading_rad);
      vx = (float) m_desired_speed * cos (m_desired_heading_rad);
      yaw = (float) m_desired_heading_rad;
      

      system_id = SYSTEM_ID;
      component_id = COMPONENT_ID;
      target_system = TARGET_SYSTEM;
      target_component = TARGET_COMPONENT;
      coordinate_frame = COORDINATE_FRAME;
      type_mask = TYPE_MASK; 

      time_boot_ms = (uint32_t) (get_time_usec()/1000);
      
      char buf[300];
      uint16_t length = mavlink_msg_set_position_target_local_ned_pack(system_id,component_id,&m_mavlink_msg,
                                                                        time_boot_ms, target_system,target_component, 
                                                                        coordinate_frame,type_mask, x, y, z, 
                                                                        vx,  vy,  vz,  afx,  afy,  afz,  yaw,  yaw_rate);
      

      unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buf, &m_mavlink_msg);

      //send binary format to MOOSDB
      Notify("MAVLINK_MSG_SET_POSITION_TARGET",(void*)buf,len);

      //debug of mavlink_message_t for confirmation
      /*********************************************/
      if (DEBUG){
        uint32_t test_time = mavlink_msg_set_position_target_local_ned_get_time_boot_ms(&m_mavlink_msg);
        Notify("VERIFY_TIME",test_time);

        uint8_t test_frame = mavlink_msg_set_position_target_local_ned_get_coordinate_frame(&m_mavlink_msg);
        Notify("VERIFY_FRAME",test_frame);

        float test_vx = mavlink_msg_set_position_target_local_ned_get_vx(&m_mavlink_msg);
        Notify("VERIFY_VX",test_vx);

        float test_vy = mavlink_msg_set_position_target_local_ned_get_vy(&m_mavlink_msg);
        Notify("VERIFY_VY",test_vy);

        float test_yaw = mavlink_msg_set_position_target_local_ned_get_yaw(&m_mavlink_msg);
        Notify("VERIFY_YAW",test_yaw);
      }
      /***********************************************/
    }else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
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
  Register("DESIRED_SPEED",DEFAULT_REGISTER_RATE);
  Register("DESIRED_HEADING",DEFAULT_REGISTER_RATE);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MavlinkConverter::buildReport() 
{
  // m_msgs << "============================================ \n";
  // m_msgs << "pMavlinkConverter                                        \n";
  // m_msgs << "============================================ \n";

  // ACTable actab(4);
  // actab << "Alpha | Bravo | Charlie | Delta";
  // actab.addHeaderLines();
  // actab << "one" << "two" << "three" << "four";
  // m_msgs << actab.getFormattedString();

  return(true);
}




