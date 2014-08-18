cmake_minimum_required(VERSION 3.0)
project(gtest_builder C CXX)
include(ExternalProject)

ExternalProject_Add(googletest
    SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk
    CMAKE_ARGS -DCMAKE_CXX_FLAGS=${MSVC_COMPILER_DEFS}
               -Dgtest_force_shared_crt=ON
     PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    # Disable install step
    INSTALL_COMMAND ""
)

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIRS ${source_dir}/include PARENT_SCOPE)

# Setup the gtest libs, as an interface
ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBS_DIR ${binary_dir})
set(GTEST_LIBS_DIR ${GTEST_LIBS_DIR} PARENT_SCOPE)

add_library(gtest INTERFACE)
target_include_directories(gtest INTERFACE ${GTEST_INCLUDE_DIR})
if( NOT WIN32 )
	target_link_libraries(gtest INTERFACE ${GTEST_LIBS_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES} )
	target_link_libraries(gtest INTERFACE ${GTEST_LIBS_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES} )
else()
target_link_libraries(gtest INTERFACE
	       debug ${GTEST_LIBS_DIR}/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
	       optimized ${GTEST_LIBS_DIR}/Release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
	)
	target_link_libraries(gtest INTERFACE
	       debug ${GTEST_LIBS_DIR}/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
	       optimized ${GTEST_LIBS_DIR}/Release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${CMAKE_FIND_LIBRARY_SUFFIXES}
	)
endif()
#now, we can call add_executable(stuff, EXTERNAL_DEPENDENCIES gtest)
