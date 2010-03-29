CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtaformats CXX)

ADD_SUBDIRECTORY(gtaformats)

SET(SQUISH_INCLUDE_PATH '' CACHE PATH "Directory containing libsquish header files.")
SET(SQUISH_LIB_PATH 'squish' CACHE PATH "Path to (or name of) libsquish library file")

IF(NOT SQUISH_LIB_PATH)
	SET(SQUISH_LIB_PATH squish)
ENDIF(NOT SQUISH_LIB_PATH)

INCLUDE_DIRECTORIES(${SQUISH_INCLUDE_PATH})

ADD_LIBRARY(gtaformats STATIC ${SRCS})
TARGET_LINK_LIBRARIES(gtaformats ${SQUISH_LIB_PATH})

INSTALL(TARGETS gtaformats DESTINATION lib)
INSTALL(	DIRECTORY gtaformats
		DESTINATION include
		FILES_MATCHING
			PATTERN *.h
			PATTERN .svn EXCLUDE
	)

