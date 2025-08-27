# CMakeList.txt : CMake project for cmake-low-level-keypress, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (KeyPress "cmake-low-level-keypress.cpp" "cmake-low-level-keypress.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET KeyPress PROPERTY CXX_STANDARD 20)
endif()

set(LIBS_DIR ${CMAKE_SOURCE_DIR}/cmake-low-level-keypress/lib)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/cmake-low-level-keypress/include)

target_include_directories(KeyPress PRIVATE ${INCLUDE_DIR})
target_link_libraries(KeyPress PRIVATE "${LIBS_DIR}/interception.lib")

set(INTERCEPTION_DLL "${CMAKE_SOURCE_DIR}/cmake-low-level-keypress/lib/interception.dll")
add_custom_command(TARGET KeyPress POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_if_different
					   "${INTERCEPTION_DLL}"
					   $<TARGET_FILE_DIR:KeyPress>)