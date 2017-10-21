/*************************************************************/
/*    NAME: Muthukumaran Chandrasekaran and Michael Novitzky */
/*    ORGN: MIT                                              */
/*    FILE: MavlinkConverter.h                               */
/*    DATE: Oct 12th 2017                                    */
/*************************************************************/

#ifndef MavlinkConverter_HEADER
#define MavlinkConverter_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "mavlink.h"
// #include "mavlink_msg_set_position_target_global_int.h"

class MavlinkConverter : public AppCastingMOOSApp
{
 public:
   MavlinkConverter();
   ~MavlinkConverter();

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
  mavlink_message_t m_mavlink_msg;
  uint8_t system_id;
  uint8_t component_id;
  uint32_t time_boot_ms;
  uint8_t target_system;
  uint8_t target_component;
  uint8_t coordinate_frame;
  uint16_t type_mask;
  float x;
  float y;
  float z;
  float vx;
  float vy;
  float vz;
  float afx;
  float afy;
  float afz;
  float yaw;
  float yaw_rate;
  float m_desired_speed;
  float m_desired_heading_deg;
  float m_desired_heading_rad;
};

#endif 
