CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtaimg CXX)

ADD_SUBDIRECTORY(src)

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH ${prj-gtaformats_SOURCE_DIR} CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ELSE(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH '' CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ENDIF(prj-gtaformats_SOURCE_DIR)
    
SET(GTAFORMATS_LIB_PATH 'gtaformats' CACHE PATH "Path to (or name of) the libgtaformats library file")
SET(BOOST_INCLUDE_PATH '' CACHE PATH "Path to the Boost include dir")
SET(BOOST_REGEX_LIB_PATH 'boost_regex' CACHE PATH "Path to (or name of) the Boost::RegEx library file")

IF(NOT GTAFORMATS_LIB_PATH)
	SET(GTAFORMATS_LIB_PATH gtaformats)
ENDIF(NOT GTAFORMATS_LIB_PATH)
IF(NOT BOOST_REGEX_LIB_PATH)
	SET(BOOST_REGEX_LIB_PATH boost_regex)
ENDIF(NOT BOOST_REGEX_LIB_PATH)

INCLUDE_DIRECTORIES(${GTAFORMATS_INCLUDE_PATH} ${BOOST_INCLUDE_PATH})

ADD_EXECUTABLE(gtaimg ${SRCS})
TARGET_LINK_LIBRARIES(gtaimg ${GTAFORMATS_LIB_PATH} ${BOOST_REGEX_LIB_PATH})

INSTALL(TARGETS gtaimg DESTINATION bin)
