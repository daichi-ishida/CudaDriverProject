cmake_minimum_required(VERSION 3.17)

find_package(CUDAToolkit 11 REQUIRED)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

function(CUDA_CONVERT_FLAGS EXISTING_TARGET)
    get_property(old_flags TARGET ${EXISTING_TARGET} PROPERTY INTERFACE_COMPILE_OPTIONS)
    if(NOT "${old_flags}" STREQUAL "")
        string(REPLACE ";" "," CUDA_flags "${old_flags}")
        set_property(TARGET ${EXISTING_TARGET} PROPERTY INTERFACE_COMPILE_OPTIONS
            "$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CXX>>:${old_flags}>$<$<BUILD_INTERFACE:$<COMPILE_LANGUAGE:CUDA>>:-Xcompiler=${CUDA_flags}>"
            )
    endif()
endfunction()

# ------------------------------------------------------------------------------
# Configure PTX Projects
# ------------------------------------------------------------------------------
set(CUDA_FILES)
add_subdirectory("kernels")

add_library(${PROJECT_NAME_PTX} OBJECT ${CUDA_FILES})
set_target_properties(${PROJECT_NAME_PTX} PROPERTIES CUDA_PTX_COMPILATION ON)
target_compile_features(${PROJECT_NAME_PTX} PUBLIC cuda_std_14)
target_compile_options(${PROJECT_NAME_PTX} PRIVATE 
--use_fast_math
"$<$<COMPILE_LANGUAGE:CUDA>:SHELL:-gencode arch=compute_52,code=sm_52>"
)
target_include_directories(${PROJECT_NAME_PTX}
    PRIVATE $<TARGET_PROPERTY:INTERFACE_INCLUDE_DIRECTORIES> ${COMMON_INCLUDE_DIRS})


# ------------------------------------------------------------------------------
# Configure Main Projects
# ------------------------------------------------------------------------------
add_executable(${PROJECT_NAME_APP})
set_property(TARGET ${PROJECT_NAME_APP} PROPERTY OUTPUT_NAME ${PROJECT_NAME})

file(GLOB_RECURSE CXX_HEADERS *.h *.hpp)
file(GLOB_RECURSE CXX_SOURCES *.cpp)
set(CXX_FILES ${CXX_HEADERS} ${CXX_SOURCES})
target_sources(${PROJECT_NAME_APP} PRIVATE ${CXX_FILES})

add_dependencies(${PROJECT_NAME_APP} ${PROJECT_NAME_PTX})

target_link_libraries(${PROJECT_NAME_APP} CUDA::cuda_driver OpenGL::GL glfw)
target_include_directories(${PROJECT_NAME_APP} PRIVATE ${COMMON_INCLUDE_DIRS})

# ------------------------------------------------------------------------------
# Set compiler options
# ------------------------------------------------------------------------------

target_compile_features(${PROJECT_NAME_APP} PUBLIC cxx_std_14)
target_compile_options(${PROJECT_NAME_APP} PUBLIC
    # warnings
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic>
    $<$<CXX_COMPILER_ID:GCC>:-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>

    # flags for each build type
    $<$<CXX_COMPILER_ID:Clang>:
        $<$<CONFIG:Release>:-O3>                  # Release
        $<$<CONFIG:Debug>:-O0 -g>                 # Debug
        $<$<CONFIG:RelWithDebgInfo>:-O3 -g>       # RelWithDebInfo
    >
    $<$<CXX_COMPILER_ID:GCC>:
        $<$<CONFIG:Release>:-O3>                  # Release
        $<$<CONFIG:Debug>:-O0 -g>                 # Debug
        $<$<CONFIG:RelWithDebgInfo>:-O3 -g>       # RelWithDebInfo
    >

    $<$<CXX_COMPILER_ID:MSVC>: 
        $<$<CONFIG:Release>:/O2 /Zi /MP>          # Release
        $<$<CONFIG:Debug>:/Od /ZI /MP>            # Debug
        $<$<CONFIG:RelWithDebgInfo>:/O2 /ZI /MP>  # RelWithDebInfo
    >
)

target_compile_definitions(${PROJECT_NAME_APP} PUBLIC
    $<$<CONFIG:Debug>:DEBUG>          # Debug
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>  # else
)

# ------------------------------------------------------------------------------
# Post Process (copy shared files)
# ------------------------------------------------------------------------------

# These are run every time the main project is built:
add_custom_target(${PROJECT_NAME} ALL DEPENDS copiedPTX.stamp copiedOptiX.stamp copiedGVDB.stamp)
# Tell Visual Studio how to start the application:
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME_APP}>
    VS_DEBUGGER_COMMAND $<TARGET_FILE:${PROJECT_NAME_APP}>)
    
add_custom_command(TARGET ${PROJECT_NAME_APP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_OBJECTS:${PROJECT_NAME_PTX}> $<TARGET_FILE_DIR:${PROJECT_NAME_APP}> COMMAND_EXPAND_LISTS)
