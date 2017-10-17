#--------------------------------------------------------
# The CMakeLists.txt for:                       iArduSubComms
# Author(s):                              Muthukumaran Chandrasekaran
#--------------------------------------------------------
INCLUDE_DIRECTORIES(../../thirdparty/common)

SET(SRC
  ArduSubComms.cpp
  ArduSubComms_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iArduSubComms ${SRC})

TARGET_LINK_LIBRARIES(iArduSubComms
   ${MOOS_LIBRARIES}
   boost_system
   apputil
   mbutil
   m
   pthread)

