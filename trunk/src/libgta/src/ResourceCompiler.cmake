# Resource Compiler
#
# This module provides a simple cross-platform resource compiler. It creates a C-header file for each input file containing a static array
# with the file's contents. This way, files can be linked into the executable and read using the header. To generate the headers, a simple
# C++ program (rc.cpp) is used. In cross-compilation mode, because we can't run the executables built by CMake, we fall back to using a
# CMake implementation of the resource compiler, which is much slower than the C++ version.
# The directory containing the generated header files is automatically added to the program include path, so you include resource headers
# like so:
#	#include <res_ALIAS.h>
#
#
# CREATE_RESOURCE(RESFILE ALIAS)
#	Creates a header file called "res_${ALIAS}.h" containing the contents of the file RESFILE in an unsigned int array with name
#	"res_${ALIAS}_data".


SET(RES_DESTDIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
FILE(MAKE_DIRECTORY "${RES_DESTDIR}")
INCLUDE_DIRECTORIES(${RES_DESTDIR})

MACRO(CREATE_RESOURCE RESFILE ALIAS)
	SET(DESTFILE "${RES_DESTDIR}/res_${ALIAS}.h")
	SET(SRCFILE "${CMAKE_CURRENT_SOURCE_DIR}/${RESFILE}")
	SET(RES_CPP_COMPILER "${CMAKE_CURRENT_SOURCE_DIR}/rc.cpp")
	
	SET(RES_CPPCOMPILER_SUCCESS OFF)
	
	IF(NOT CMAKE_CROSSCOMPILING AND EXISTS "${RES_CPP_COMPILER}")
		# Use the resource compiler written in C++.
		TRY_RUN(RES_RUN_RESULT RES_COMPILE_RESULT ${CMAKE_BINARY_DIR} "${RES_CPP_COMPILER}"
		        COMPILE_OUTPUT_VARIABLE RES_COMPILE_OUTPUT
			RUN_OUTPUT_VARIABLE RES_RUN_OUTPUT
		        ARGS "\"${SRCFILE}\"" "\"${DESTFILE}\"" "\"${ALIAS}\"")
		
		IF (NOT RES_COMPILE_RESULT)
			MESSAGE(SEND_ERROR "Compiling the resource compiler itself failed! Will use CMake implementation instead. Compiler log:\n**********\n${RES_COMPILE_OUTPUT}\n**********")
		ELSEIF(NOT "${RES_RUN_OUTPUT}" MATCHES "^Success$")
			MESSAGE(SEND_ERROR "Running the resource compiler on file ${RESFILE} failed. Will use CMake implementation instead. Output:\n**********\n${RES_RUN_OUTPUT}\n**********")
		ELSE(NOT RES_COMPILE_RESULT)
			SET(RES_CPPCOMPILER_SUCCESS ON)
		ENDIF (NOT RES_COMPILE_RESULT)
	ENDIF(NOT CMAKE_CROSSCOMPILING AND EXISTS "${RES_CPP_COMPILER}")

	IF(NOT RES_CPPCOMPILER_SUCCESS)
		# In cross-compilation mode we can't execute the binaries we build, so we can't use TRY_RUN to execute the C++ resource compiler.
		# In this case, we have to use a CMake-only resource compiler, which is much slower than the C++ version, but it works.

		#IF(NOT CMAKE_CROSSCOMPILING)
		#	MESSAGE("Warning: C++ resource compiler rc.cpp not found, using CMake implementation.")
		#ENDIF(NOT CMAKE_CROSSCOMPILING)

		STRING(TOUPPER "${ALIAS}" UPPER_ALIAS)

		FILE(WRITE "${DESTFILE}" "")
		FILE(APPEND "${DESTFILE}" "// Automatically compiled from resource file ${SRCFILE}\n")
		FILE(APPEND "${DESTFILE}" "// DO NOT EDIT THIS FILE! CHANGES WILL BE LOST UPON RECOMPILATION!\n\n")

		FILE(APPEND "${DESTFILE}" "#ifndef RES_${UPPER_ALIAS}_H_\n")
		FILE(APPEND "${DESTFILE}" "#define RES_${UPPER_ALIAS}_H_\n")

		FILE(APPEND "${DESTFILE}" "static const unsigned char res_${ALIAS}_data[] = {")
		
		FILE(READ "${SRCFILE}" RESDATA HEX)

		STRING(LENGTH "${RESDATA}" RESDATA_LEN)
		MATH(EXPR RESDATA_BYTES "${RESDATA_LEN}/2-1")

		FOREACH(RESDATA_BYTEOFF RANGE ${RESDATA_BYTES})
			MATH(EXPR RESDATA_MOD "${RESDATA_BYTEOFF}%20")

			IF(${RESDATA_MOD} EQUAL 0)
				FILE(APPEND "${DESTFILE}" "\n\t")
			ENDIF(${RESDATA_MOD} EQUAL 0)

			MATH(EXPR RESDATA_STROFF "${RESDATA_BYTEOFF}*2")
			STRING(SUBSTRING "${RESDATA}" ${RESDATA_STROFF} 2 RESDATA_BYTE)
			FILE(APPEND "${DESTFILE}" "0x${RESDATA_BYTE}, ")
		ENDFOREACH(RESDATA_BYTEOFF)

		FILE(APPEND "${DESTFILE}" "\n};\n")
		FILE(APPEND "${DESTFILE}" "#endif\n")
	ENDIF(NOT RES_CPPCOMPILER_SUCCESS)

	SET(RESOURCE_FILES ${RESOURCE_FILES} "${DESTFILE}")
ENDMACRO(CREATE_RESOURCE)
