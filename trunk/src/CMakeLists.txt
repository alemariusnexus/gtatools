CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gtatools CXX)

ADD_SUBDIRECTORY(libgtaformats/src libgtaformats/build EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(gtaimg/src gtaimg/build EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(gtatxd/src gtatxd/build EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(gtatools-gui/src gtatools-gui/build EXCLUDE_FROM_ALL)
