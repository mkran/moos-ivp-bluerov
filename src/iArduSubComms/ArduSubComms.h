/************************************************************/
/*    NAME: Muthukumaran Chandrasekaran                     */
/*    ORGN: MIT Cambridge MA                                */
/*    FILE: ArduSubComms.h                                  */
/*    DATE: October 13th, 2017                              */
/************************************************************/

#ifndef ArduSubComms_HEADER
#define ArduSubComms_HEADER

#include <boost/asio.hpp>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "mavlink.h"

class ArduSubComms : public AppCastingMOOSApp
{
 public:
   ArduSubComms();
   ~ArduSubComms();

 protected: // Standard MOOSApp functions to overload
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables

 private: // State variables
   std::string*                                m_mavlink_msg;
   std::string                                 m_mavlink_port;
   unsigned int                                m_mavlink_baud;
   boost::asio::io_service                     m_io;
   boost::shared_ptr<boost::asio::serial_port> m_serial;

};

#endif
