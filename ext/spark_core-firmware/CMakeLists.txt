cmake_minimum_required(VERSION 3.0)
project(spark_core_firmware_builder C CXX)
include(ExternalProject)

ExternalProject_Add(sparkcorefirmware_util
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    #GIT_REPOSITORY https://github.com/spark/core-firmware
	#use my branch for MSVC fixes
	GIT_REPOSITORY https://github.com/johnm1019/core-firmware
    #Disable some steps
    #UPDATE_COMMAND ""
    #CONFIGURE_COMMAND ""
	# No sense in building this against ARM stuff if we want to debug in MSVC
	#BUILD_IN_SOURCE 1
	#BUILD_COMMAND cd build && ${GNU_MAKE_DIR}/mingw32-make  
	#				GCC_PREFIX=${ARM_EABI_GCC_DIR}/arm-eabi-
	#				LIB_CORE_COMMON_PATH=../../../spark_core-common-lib/src/spark_core_common_lib
	#				LIB_CORE_COMMUNICATION_PATH=../../../core-communication-lib/src/core-communication-lib
	#				LIB_CORE_LIBRARIES_PATH=libraries/
	#				"RM=del /F"
	#				"MKDIR=${GNU_MSYS_CORE_DIR}/mkdir -p"
    INSTALL_COMMAND ""
)

# Specify include dir
ExternalProject_Get_Property(sparkcorefirmware_util source_dir)
set(SPARK_FIRMWARE_INCLUDE_DIRS ${source_dir}/inc)
set(SPARK_FIRMWARE_INCLUDE_DIRS ${SPARK_FIRMWARE_INCLUDE_DIRS} PARENT_SCOPE)

set(SPARK_FIRMWARE_SRC_DIRS ${source_dir}/src)
set(SPARK_FIRMWARE_SRC_DIRS ${SPARK_FIRMWARE_SRC_DIRS} PARENT_SCOPE)

#specify link dir
ExternalProject_Get_Property(sparkcorefirmware_util binary_dir)
set(SPARK_FIRMWARE_LIBS_DIRS ${binary_dir})
set(SPARK_FIRMWARE_LIBS_DIRS ${SPARK_FIRMWARE_LIBS_DIRS} PARENT_SCOPE)

#add includes and libs
add_library(spark_core_firmware_interface INTERFACE)
target_include_directories(spark_core_firmware_interface INTERFACE ${SPARK_FIRMWARE_INCLUDE_DIRS})
if( NOT WIN32 )
	target_link_libraries(spark_core_firmware_interface INTERFACE ${SPARK_FIRMWARE_LIBS_DIRS}/${CMAKE_FIND_LIBRARY_PREFIXES}spark_wiring_lib${CMAKE_FIND_LIBRARY_SUFFIXES} )
else()
target_link_libraries(spark_core_firmware_interface INTERFACE
	       debug ${SPARK_FIRMWARE_LIBS_DIRS}/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}spark_wiring_lib${CMAKE_FIND_LIBRARY_SUFFIXES}
	       optimized ${SPARK_FIRMWARE_LIBS_DIRS}/Release/${CMAKE_FIND_LIBRARY_PREFIXES}spark_wiring_lib${CMAKE_FIND_LIBRARY_SUFFIXES}
	)
endif()

