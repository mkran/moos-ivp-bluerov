/************************************************************/
/*    NAME: Muthukumaran Chandrasekaran                     */
/*    ORGN: MIT Cambridge MA                                */
/*    FILE: ArduSubComms.h                                  */
/*    DATE: October 13th, 2017                              */
/************************************************************/

#ifndef ArduSubComms_HEADER
#define ArduSubComms_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "mavlink.h"
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class UDPClient{
   public:
      //UDPClient();
      UDPClient(
         boost::asio::io_service& io_service, 
         const std::string& host, 
         const std::string& port
      ) : io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), 0)) {
         udp::resolver resolver(io_service_);
         udp::resolver::query query(udp::v4(), host, port);
         udp::resolver::iterator iter = resolver.resolve(query);
         endpoint_ = *iter;
      }

      ~UDPClient()
      {
         socket_.close();
      }

      void send(const std::string& msg) {
         socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
      }

   private:
      boost::asio::io_service& io_service_;
      udp::socket socket_;
      udp::endpoint endpoint_;
};

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
   std::string                                 m_mavlink_host;
   std::string                                 m_mavlink_port;
   unsigned int                                m_mavlink_baud;
   boost::asio::io_service                     m_io;
   boost::shared_ptr<boost::asio::serial_port> m_serial;
   boost::shared_ptr<udp::socket>              m_udp;
   // UDPClient client(boost::asio::io_service m_io, std::string m_mavlink_host, std::string m_mavlink_port);
   UDPClient *client;
};

#endif
