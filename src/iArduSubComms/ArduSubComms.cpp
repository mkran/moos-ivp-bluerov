/************************************************************/
/*    NAME: Muthukumaran Chandrasekaran                     */
/*    ORGN: MIT Cambridge MA                                */
/*    FILE: ArduSubComms.cpp                                */
/*    DATE: October 13th, 2017                              */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "ArduSubComms.h"

// Max rate to receive MOOS updates (0 indicates no rate, get all)
#define DEFAULT_REGISTER_RATE 0.0

// Enable/disable debug code
#define DEBUG 1

using namespace std;

//---------------------------------------------------------
// Constructor

ArduSubComms::ArduSubComms()
{
  m_mavlink_msg = new std::string();
  m_mavlink_port = "/dev/ttyUSB0";
  m_mavlink_baud = 115200;
}

//---------------------------------------------------------
// Destructor

ArduSubComms::~ArduSubComms()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ArduSubComms::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

    if(p->IsName("MAVLINK_MSG_SET_POSITION_TARGET_GLOBAL_INT")) {
      m_mavlink_msg = new string((char*)p->GetBinaryData(), p->GetBinaryDataSize());
      boost::asio::write(*m_serial, boost::asio::buffer(m_mavlink_msg->c_str(), m_mavlink_msg->size()));

      //debug of mavlink_message_t for confirmation
      /*********************************************/
      if (DEBUG){
        mavlink_message_t mavlink_msg_buf; //INSTANTIATE THE BINARY MAVLINK MESSAGE BUFFER HERE TO TEST
        uint8_t test_frame = mavlink_msg_set_position_target_global_int_get_coordinate_frame(&mavlink_msg_buf);
        Notify("VERIFY_FRAME",test_frame);

        float test_vx = mavlink_msg_set_position_target_global_int_get_vx(&mavlink_msg_buf);
        Notify("VERIFY_VX",test_vx);

        uint32_t test_time = mavlink_msg_set_position_target_global_int_get_time_boot_ms(&mavlink_msg_buf);
        Notify("VERIFY_TIME",test_time);

        float test_yaw = mavlink_msg_set_position_target_global_int_get_yaw(&mavlink_msg_buf);
        Notify("VERIFY_YAW",test_yaw);
      }
      /***********************************************/
      
      delete m_mavlink_msg;
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

    if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ArduSubComms::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool ArduSubComms::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool ArduSubComms::OnStartUp()
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

  m_serial = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(m_io, m_mavlink_port));
  m_serial->set_option(boost::asio::serial_port_base::baud_rate(m_mavlink_baud));

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void ArduSubComms::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("MAVLINK_MSG_SET_POSITION_TARGET_GLOBAL_INT", DEFAULT_REGISTER_RATE);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool ArduSubComms::buildReport()
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




