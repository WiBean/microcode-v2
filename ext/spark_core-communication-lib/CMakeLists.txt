cmake_minimum_required(VERSION 3.0)
project(spark_core_communication_lib_builder C CXX)
include(ExternalProject)

ExternalProject_Add(spark_core_communication_lib
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    #GIT_REPOSITORY https://github.com/spark/core-communication-lib
	#use my branch for MSVC fixes
	GIT_REPOSITORY https://github.com/johnm1019/core-communication-lib
    #DOWNLOAD_COMMAND "git pull"
    #BUILD_COMMAND "cmake --build ."
    #Disable some steps
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

# Specify include dir
ExternalProject_Get_Property(spark_core_communication_lib source_dir)
set(SPARK_COMMUNICATION_INCLUDE_DIRS ${source_dir}/src
							${source_dir}/lib/tropicssl/include 
							PARENT_SCOPE)
#update this if needed in the future
set(SPARK_COMMUNICATION_SRC_DIRS ${source_dir}/src
						PARENT_SCOPE)

# Specify spark_core_communication_lib link libraries
#ExternalProject_Get_Property(spark_core_communication_lib binary_dir)
#set(SPARK_LIBS_DIR ${binary_dir} PARENT_SCOPE)
