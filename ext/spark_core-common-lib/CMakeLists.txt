cmake_minimum_required(VERSION 3.0)
project(spark_core_common_lib_builder C CXX)
include(ExternalProject)

ExternalProject_Add(spark_core_common_lib
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    #GIT_REPOSITORY https://github.com/spark/core-common-lib
	#use my branch for MSVC fixes
	GIT_REPOSITORY https://github.com/johnm1019/core-common-lib
    #GIT_TAG origin/master
    #DOWNLOAD_COMMAND "git pull"
    #BUILD_COMMAND "cmake --build ."
    #Disable some steps
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

# Specify include dir
ExternalProject_Get_Property(spark_core_common_lib source_dir)
set(SPARK_COMMON_INCLUDE_DIRS ${source_dir}/CMSIS/Device/ST/STM32F10x/Include
							${source_dir}/CMSIS/Include
							${source_dir}/SPARK_Firmware_Driver/inc
							${source_dir}/STM32F10x_StdPeriph_Driver/inc
							${source_dir}/STM32_USB-FS-Device_Driver/inc
							${source_dir}/CC3000_Host_Driver
							PARENT_SCOPE)
#update this if needed in the future
set(SPARK_COMMON_SRC_DIRS ${source_dir}/SPARK_Firmware_Driver/src
						${source_dir}/STM32F10x_StdPeriph_Driver/src
						${source_dir}/STM32_USB-FS-Device_Driver/src
						${source_dir}/CC3000_Host_Driver
						PARENT_SCOPE)

# Specify MainTest's link libraries
#ExternalProject_Get_Property(spark_core_firmware binary_dir)
#set(SPARK_LIBS_DIR ${binary_dir} PARENT_SCOPE)
