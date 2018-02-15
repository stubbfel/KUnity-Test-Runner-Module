cmake_minimum_required(VERSION 2.6)

include("dev-utility/CMake-Module/BuildKernelModule/BuildKernelModule.cmake")
SET(module_name kunity_test_runner_module)
project (${module_name})

if(NOT KERNEL_DIR)
Set(KERNEL_DIR "/lib/modules/${CMAKE_SYSTEM_VERSION}/build" )
endif()

set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -Weverything")

file(GLOB module_src_files "${PROJECT_SOURCE_DIR}/src/*.c")

include_directories(
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/lib/KUnity/src")

add_module("${module_name}" "${module_src_files}" "${KERNEL_DIR}")

if(BUILD_TEST_MOUDLE)
## test module section
    Set(TEST_MODULE_SOURCE_DIR  "${PROJECT_BINARY_DIR}/test_${module_name}")

    file(GLOB module_test_files
        ${module_src_files}
        "${PROJECT_SOURCE_DIR}/test/src/*.c"
        "${PROJECT_SOURCE_DIR}/lib/KUnity/src/*.c"
        "${PROJECT_SOURCE_DIR}/lib/KUnity/lib/Unity/src/*.c")

    include_directories(
        "${PROJECT_SOURCE_DIR}/test/src"
        "${PROJECT_SOURCE_DIR}/lib/KUnity/lib/Unity/src")

    add_definitions(-DUNITY_INCLUDE_CONFIG_H)
    add_module("test_${module_name}" "${module_test_files}" "${KERNEL_DIR}")
endif()
