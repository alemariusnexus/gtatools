CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gtatxd CXX)

ADD_EXECUTABLE(gtatxd cliarg ExtractVisitor ListVisitor main)
TARGET_LINK_LIBRARIES(gtatxd boost_regex gtaformats squish png)

INSTALL(TARGETS gtatxd DESTINATION bin)

