cmake_minimum_required(VERSION 2.6)

include("test/lib/KUnity/dev-utility/BuildKernelModule.cmake")
SET(module_name kunity_test_runner_module)
project (${module_name})

Set(KERNEL_DIR "/lib/modules/${CMAKE_SYSTEM_VERSION}/build" )
set(CMAKE_C_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -Weverything")

file(GLOB module_src_files
    "${PROJECT_SOURCE_DIR}/src/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.c")

set(modules_includes
    "${KERNEL_DIR}/include" "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/src")
add_module("${module_name}" "${module_src_files}" "${modules_includes}" "" "${KERNEL_DIR}")

## test module section

Set(TEST_MODULE_SOURCE_DIR  "${PROJECT_BINARY_DIR}/test_${module_name}")

file(GLOB module_test_files
    ${module_src_files}
    "${PROJECT_SOURCE_DIR}/test/src/*.c"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/Kbuild"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/src/*.h"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/src/*.c"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/lib/fff/fff.h"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/lib/Unity/src/*.h"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/lib/Unity/src/*.c")

file(GLOB test_modules_includes
    ${modules_includes}
    "${PROJECT_SOURCE_DIR}/test/src"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/src"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/lib/fff"
    "${PROJECT_SOURCE_DIR}/test/lib/KUnity/lib/Unity/src")

set(test_modules_definitions -DUNITY_INCLUDE_CONFIG_H)
add_module("test_${module_name}" "${module_test_files}" "${test_modules_includes}" "${test_modules_definitions}" "${KERNEL_DIR}")
