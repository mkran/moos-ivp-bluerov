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
#define DEBUG 0

// TOGGLE_PORT 1 for Enabling Comms via UDP (for BlueROV2 control (or in SITL) from MOOS running on GCS)
// TOGGLE_PORT 0 for Enabling Comms via SERIAL Port (for BlueROV2 control from MOOS running on companion computer onboard)
#define TOGGLE_PORT 1

using namespace std;

//---------------------------------------------------------
// Constructor

ArduSubComms::ArduSubComms()
{
  if(TOGGLE_PORT){
    m_mavlink_host = "127.0.0.1";
    m_mavlink_port = "14000";
  }else{
    m_mavlink_port = "/dev/ttyACM0";
    m_mavlink_baud = 115200;
  }
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

    if(p->IsName("MAVLINK_MSG_SET_POSITION_TARGET")) {
      memcpy((void*)&m_mavlink_msg, (void*)p->GetBinaryData(), p->GetBinaryDataSize());

      if(TOGGLE_PORT){
        m_udp_client->send(&m_mavlink_msg);
      }else{
        boost::asio::write(*m_serial, boost::asio::buffer(&m_mavlink_msg, p->GetBinaryDataSize()));
      }


      //debug of mavlink_message_t for confirmation
      /*********************************************/
      if (DEBUG){
        Notify("VERIFY_MSG",(void*)&m_mavlink_msg, p->GetBinaryDataSize());
      }
      /***********************************************/

    }else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);

    #if 0 // Keep these around just for template
      string comm  = msg.GetCommunity();
      double dval  = msg.GetDouble();
      string sval  = msg.GetString();
      string msrc  = msg.GetSource();
      double mtime = msg.GetTime();
      bool   mdbl  = msg.IsDouble();
      bool   mstr  = msg.IsString();
    #endif

    
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

  if(TOGGLE_PORT){
    m_udp_client = new UDPClient(m_io, m_mavlink_host, m_mavlink_port);
  }else{
    m_serial = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(m_io, m_mavlink_port));
    m_serial->set_option(boost::asio::serial_port_base::baud_rate(m_mavlink_baud));
  }

  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void ArduSubComms::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("MAVLINK_MSG_SET_POSITION_TARGET", DEFAULT_REGISTER_RATE);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool ArduSubComms::buildReport()
{
  // m_msgs << "============================================ \n";
  // m_msgs << "File:                                        \n";
  // m_msgs << "============================================ \n";

  // ACTable actab(4);
  // actab << "Alpha | Bravo | Charlie | Delta";
  // actab.addHeaderLines();
  // actab << "one" << "two" << "three" << "four";
  // m_msgs << actab.getFormattedString();

  return(true);
}




