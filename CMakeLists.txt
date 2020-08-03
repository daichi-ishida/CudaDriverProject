cmake_minimum_required(VERSION 3.17)
project(test_all)

# ------------------------------------------------------------------------------
# Set Find.cmake directory
# ------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


# ------------------------------------------------------------------------------
# Set build options
# ------------------------------------------------------------------------------
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(MSVC)
  set(CMAKE_C_FLAGS_DEBUG "/Od /DDEBUG /ZI /MP")
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /DDEBUG /ZI /MP")
  set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG /Zi /MP")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /Zi /MP")

  # set_property(GLOBAL APPEND PROPERTY LINK_FLAGS_DEBUG /DEBUG:FASTLINK)
  # set_property(GLOBAL APPEND PROPERTY LINK_FLAGS_RELEASE /DEBUG:FASTLINK)
else()
  set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# ------------------------------------------------------------------------------
# Setup additional libraries
# ------------------------------------------------------------------------------
find_package(CUDAToolkit 11 REQUIRED)
if(NOT DEFINED CMAKE_CUDA_STANDARD)
    set(CMAKE_CUDA_STANDARD 14)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()

# find_package(XXX REQUIRED)
set(ALL_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/common")
set(ALL_LIBRARIES)

# ------------------------------------------------------------------------------
# Add subdirectory
# ------------------------------------------------------------------------------
add_subdirectory("src/display" "display")
