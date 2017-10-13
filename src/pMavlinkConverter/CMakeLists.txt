#--------------------------------------------------------
# The CMakeLists.txt for:                       pMavlinkConverter
# Author(s): Muthukumaran Chandrasekaran and Michael Novitzky                             
#--------------------------------------------------------
INCLUDE_DIRECTORIES(../../thirdparty/common)

SET(SRC
  MavlinkConverter.cpp
  MavlinkConverter_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pMavlinkConverter ${SRC})

TARGET_LINK_LIBRARIES(pMavlinkConverter
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

