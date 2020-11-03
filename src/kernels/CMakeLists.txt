# ------------------------------------------------------------------------------
# Configure PTX Projects
# ------------------------------------------------------------------------------
file(GLOB CUDA_HEADERS *.cuh)
file(GLOB CUDA_SOURCES *.cu)

set(CUDA_KERNEL_FILES ${CUDA_HEADERS} ${CUDA_SOURCES})

add_library(${PROJECT_NAME_PTX} OBJECT ${CUDA_KERNEL_FILES})
set_target_properties(${PROJECT_NAME_PTX} PROPERTIES CUDA_PTX_COMPILATION ON)
target_compile_features(${PROJECT_NAME_PTX} PUBLIC cuda_std_14)
target_compile_options(${PROJECT_NAME_PTX} PRIVATE 
--use_fast_math
"$<$<COMPILE_LANGUAGE:CUDA>:SHELL:-gencode arch=compute_52,code=sm_52>"
)
target_include_directories(${PROJECT_NAME_PTX}
    PRIVATE $<TARGET_PROPERTY:INTERFACE_INCLUDE_DIRECTORIES> ${COMMON_INCLUDE_DIRS})

