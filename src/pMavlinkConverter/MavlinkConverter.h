/*************************************************************/
/*    NAME: Muthukumaran Chandrasekaran and Michael Novitzky */
/*    ORGN: MIT                                              */
/*    FILE: MavlinkConverter.h                               */
/*    DATE: Oct 12th 2017                                    */
/*************************************************************/

#ifndef MavlinkConverter_HEADER
#define MavlinkConverter_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

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
};

#endif 
