cmake_minimum_required(VERSION 3.0)


################################
# Normal Libraries & Executables
################################

# Debug outputs for checking vars come in properly
# message(STATUS "src/SPARK_FIRMWARE_INCLUDE_DIRS: " ${SPARK_FIRMWARE_INCLUDE_DIRS})
# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
 # message(STATUS "dir='${dir}'")
# endforeach()

add_library(heatingsm_lib HeatingSM.cpp)
set_target_properties(heatingsm_lib PROPERTIES LINKER_LANGUAGE CXX)

add_library(heatingsmpid_lib HeatingSMPID.cpp)
set_target_properties(heatingsmpid_lib PROPERTIES LINKER_LANGUAGE CXX)
#for debug we may need the spark libraries here
add_dependencies(heatingsmpid_lib sparkcorefirmware_util)
target_link_libraries(heatingsmpid_lib spark_core_firmware_interface)

add_library(thermistor_lib Thermistor.cpp)
set_target_properties(thermistor_lib PROPERTIES LINKER_LANGUAGE CXX)

add_library(utilities_lib Utilities.cpp)
set_target_properties(utilities_lib PROPERTIES LINKER_LANGUAGE CXX)
#setup the dep so the git pull operations are finished first
add_dependencies(utilities_lib sparkcorefirmware_util)
target_link_libraries(utilities_lib spark_core_firmware_interface)

add_library(pid_lib Pid.cpp)
set_target_properties(pid_lib PROPERTIES LINKER_LANGUAGE CXX)

#This is a big guy
#add_library(sparkintervaltimer_lib SparkIntervalTimer.cpp)
#set_target_properties(sparkintervaltimer_lib PROPERTIES LINKER_LANGUAGE CXX)
#setup the dep so the git pull operations are finished first
#add_dependencies(sparkintervaltimer_lib spark_core_firmware)
#add_dependencies(sparkintervaltimer_lib spark_core_common_lib)
#add_dependencies(sparkintervaltimer_lib spark_core_communication_lib)
