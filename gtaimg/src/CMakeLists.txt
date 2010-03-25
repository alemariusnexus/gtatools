CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gtaimg CXX)

SET(SRCS cliarg ExtractVisitor ListVisitor main)

ADD_EXECUTABLE(gtaimg ${SRCS})
TARGET_LINK_LIBRARIES(gtaimg gtaformats boost_regex)

INSTALL(TARGETS gtaimg DESTINATION bin)

