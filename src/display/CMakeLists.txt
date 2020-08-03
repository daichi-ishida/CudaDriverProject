cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME display)
project(${PROJECT_NAME} LANGUAGES CUDA CXX C)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# Add PTX files as a library
file(GLOB CUDA_HEADERS *.cuh)
file(GLOB CUDA_SOURCES *.cu)
set(CUDA_FILES ${CUDA_HEADERS} ${CUDA_SOURCES})
set(PROJECT_PTX_NAME "${PROJECT_NAME}PTX")
message(STATUS "Build CUDA kernels: ${CUDA_FILES}")

add_library(${PROJECT_PTX_NAME} OBJECT ${CUDA_FILES})
set_target_properties(${PROJECT_PTX_NAME} PROPERTIES CUDA_PTX_COMPILATION ON)
target_include_directories(${PROJECT_PTX_NAME}
    PRIVATE $<TARGET_PROPERTY:INTERFACE_INCLUDE_DIRECTORIES>)

source_group("GPU Kernels" FILES ${CUDA_FILES})

# Main project
file(GLOB CXX_HEADERS *.h *.hpp)
file(GLOB CXX_SOURCES *.cpp)
set(CXX_FILES ${CXX_HEADERS} ${CXX_SOURCES})

add_executable(${PROJECT_NAME} ${CXX_FILES})
add_dependencies(${PROJECT_NAME} ${PROJECT_PTX_NAME})
target_link_libraries(${PROJECT_NAME} CUDA::cuda_driver OpenGL::GL glfw)
target_include_directories(${PROJECT_NAME} PRIVATE ${ALL_INCLUDE_DIRS})

source_group("Header Files" FILES ${CXX_HEADERS})
source_group("Source Files" FILES ${CXX_SOURCES})

# Tell Visual Studio to set its debugger working directory to the executable directory
set_property(TARGET ${PROJECT_NAME}
    PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)


# Finally, copy the PTX FILES to the executable directory
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/copiedPTX.stamp
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_OBJECTS:${PROJECT_PTX_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/copiedPTX.stamp
    COMMAND_EXPAND_LISTS)
set_property(SOURCE ${CXX_SOURCES} APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/copiedPTX.stamp)
