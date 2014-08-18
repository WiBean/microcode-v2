#Specify the version being used aswell as the language
cmake_minimum_required(VERSION 3.0)

# Make PROJECT_SOURCE_DIR, PROJECT_BINARY_DIR, and PROJECT_NAME available.
set(PROJECT_NAME wibean_microcode_spark)
project(${PROJECT_NAME} C CXX)

################
# Windows or GNU?
################
if(CMAKE_COMPILER_IS_GNUCXX)
        add_definitions(-Wall -ansi -Wno-deprecated -pthread -std=c++11)
endif()
if(MSVC)
#vc 2012 fix for vararg templates
    set(MSVC_COMPILER_DEFS "-D_VARIADIC_MAX=10")
endif()


######################
# Options. Turn on with 'cmake -Dmyvarname=ON'.
######################
option(TEST "Build tests." OFF) # Makes boolean 'test' available.
if( NOT MSVC )
  #assume make, g++ and gcc are in our path
  set( GNU_MAKE_DIR "" )
  #assume mkdir is in the shell
  set( GNU_MKDIR_DIR "" )
else()
  #find gnu make
  find_path(GNU_MAKE_DIR
      NAMES mingw32-make
            mingw32-make.exe
      DOC "The MinGW directory where mingw32-make lives (WIN only)"
      HINTS C:/MinGW-32/bin )  
  #find MSYS/mkdir
  find_path(GNU_MSYS_CORE_DIR
      NAMES mkdir
            mkdir.exe
      DOC "The MinGW/MSYS directory where we can find unix style mkdir"
      HINTS C:/MinGW/msys/1.0/bin )
  #find arm-eabi-gcc toolchain
  find_path(ARM_EABI_GCC_DIR
      NAMES arm-eabi-gcc
            arm-eabi-gcc.exe
      DOC "The directory containing arm-eabi-* executables compiled for windows"
      HINTS C:/SysGCC/arm-eabi/bin )  
endif()


#######################
# set common include folder for module
#######################
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/src)
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)


################################
# Spark.io libraries
################################
#include the spark core stuff
SET(SPARK_FIRMWARE_DIR ${EXT_PROJECTS_DIR}/spark_core-firmware)
#SET(SPARK_COMMON_DIR ${EXT_PROJECTS_DIR}/spark_core-common-lib)
#SET(SPARK_COMMUNICATION_DIR ${EXT_PROJECTS_DIR}/spark_core-communication-lib)
add_subdirectory(${SPARK_FIRMWARE_DIR})
#add_subdirectory(${SPARK_COMMON_DIR})
#add_subdirectory(${SPARK_COMMUNICATION_DIR})

#the common has a file which requires we define which microcontroller we are using for the spark.io -- that is STM32F10X_MD
#add_definitions(-DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER)

include_directories(${SPARK_COMMON_INCLUDE_DIRS})
#include_directories(${SPARK_COMMUNICATION_INCLUDE_DIRS})


################################
# Our sources and includes
################################
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(src)

################################
# Normal Libraries & Executables
################################

#add_executable(wibean_spark src/wibean-firmwarev2.ino)
#set_target_properties(wibean_spark PROPERTIES LINKER_LANGUAGE CXX)
add_executable(dummy_main main.cpp)
# Key idea: SEPARATE OUT your main() function into its own file so it can be its
# own executable. Separating out main() means you can add this library to be
# used elsewhere.


#setup the deps on the main
target_link_libraries(dummy_main heatingsm_lib)
target_link_libraries(dummy_main thermistor_lib)
target_link_libraries(dummy_main utilities_lib)
#target_link_libraries(dummy_main sparkintervaltimer_lib)
# Template classes don't get libs...
# this includes: PumpProgram, SortedLookupTable, AveragingFloatBuffer

################################
# Testing
################################
if (TEST)
  # This adds another subdirectory, which has 'project(gtest)'.
  add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
  enable_testing()
  # Include the gtest library. gtest_SOURCE_DIR is available due to
  # 'project(gtest)' above.
  include_directories(${GTEST_INCLUDE_DIRS})
  #our tests will need access to the other libraries
  include_directories(src)
  add_subdirectory(tests)


  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  #add_test(NAME test-utils COMMAND test-wibeanUtilities)
  #These lines added in test subdirectory cmake
endif()
