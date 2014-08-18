##############
# Unit Tests
##############

# First one as example with comments!
add_executable(test-wibeanUtilities test_utilities.cpp)
#ensure the external project is completely built first
add_dependencies(test-wibeanUtilities googletest)
# Standard linking+deps to gtest stuff.
target_link_libraries(test-wibeanUtilities gtest)
# Extra linking for the project.
target_link_libraries(test-wibeanUtilities utilities_lib)
# Use this helper so we can have one command to run tests
add_test(NAME test-wibeanUtilities COMMAND test-wibeanUtilities)


add_executable(test-sortedLookupTable test_sortedLookupTable.cpp)
add_dependencies(test-sortedLookupTable googletest)
target_link_libraries(test-sortedLookupTable gtest)
target_link_libraries(test-sortedLookupTable utilities_lib)
add_test(NAME test-sortedLookupTable COMMAND test-sortedLookupTable)
  