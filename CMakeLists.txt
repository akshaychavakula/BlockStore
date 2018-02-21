cmake_minimum_required (VERSION 2.8)
project(assignment3)

set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror -I../include")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror -I../include")

# Read some documentation, do some googling, refer to things we've given you
# cmake is popular, so someone on the internet has had your problem before


include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(bitmap SHARED src/bitmap.c)
add_library(block_store SHARED src/block_store.c)
target_link_libraries(block_store bitmap)

add_library(libblock_store SHARED src/block_store.c include/block_store.h)
set_target_properties(libblock_store PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_target_properties(bitmap PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(block_store PROPERTIES POSITION_INDEPENDENT_CODE ON)

# if the test program isn't found in the correct place, you will lose points
# Link runTests with what we want to test and the GTest and pthread Library
add_executable(${PROJECT_NAME}_test test/tests.cpp)




# Enable grad/bonus tests by setting the variable to 1
target_compile_definitions(${PROJECT_NAME}_test PRIVATE GRAD_TESTS=0)

target_link_libraries(${PROJECT_NAME}_test block_store gtest pthread)
enable_testing()
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)
