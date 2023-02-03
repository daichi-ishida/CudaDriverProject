find_package(CUDAToolkit 11 REQUIRED)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory("kernels")

# ------------------------------------------------------------------------------
# Configure Main Projects
# ------------------------------------------------------------------------------
add_executable(${PROJECT_NAME_APP})

file(GLOB_RECURSE CXX_SOURCES *.cpp)
set(CXX_FILES ${CXX_SOURCES})
target_sources(${PROJECT_NAME_APP} PRIVATE ${CXX_FILES})

set_target_properties(${PROJECT_NAME_APP} PROPERTIES CUDA_ARCHITECTURES "61")

target_link_libraries(${PROJECT_NAME_APP} CUDA::cuda_driver OpenGL::GL glfw)
target_include_directories(${PROJECT_NAME_APP} PRIVATE ${COMMON_INCLUDE_DIRS})

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

add_dependencies(${PROJECT_NAME_APP} ${PROJECT_NAME_PTX})

# ------------------------------------------------------------------------------
# Post Process (copy shared files)
# ------------------------------------------------------------------------------

add_custom_command(TARGET ${PROJECT_NAME_APP} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_OBJECTS:${PROJECT_NAME_PTX}> $<TARGET_FILE_DIR:${PROJECT_NAME_APP}> COMMAND_EXPAND_LISTS)
