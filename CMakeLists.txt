cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME evaluation_core_h7_v0)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add threadx sources
add_subdirectory(middleware/threadx)
add_subdirectory(middleware/filex)
add_subdirectory(middleware/levelx)
add_subdirectory(middleware/usbx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    examples/usbx_usd_example/threadx/app_threadx.c
    examples/usbx_usd_example/threadx/app/app_azure_rtos.c
    examples/usbx_usd_example/usbx/app/app_usbx_device.c
    examples/usbx_usd_example/usbx/app/ux_device_descriptors.c
    examples/usbx_usd_example/usbx/app/ux_device_msc.c
    examples/usbx_usd_example/app.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    examples/usbx_usd_example/threadx
    examples/usbx_usd_example/threadx/app
    examples/usbx_usd_example/usbx/target
    examples/usbx_usd_example/usbx/app
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    threadx
    levelx
    filex
    usbx
)
