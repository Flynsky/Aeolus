cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "MinSizeRel")

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME ESC)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")
# to install on linux: apt install gcc-arm-none-eabi

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

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    debugf_vcp/debugf_vcp.c
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    debugf_vcp
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Enable hardware floating-point support in the compiler (if applicable)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Add the linker flag to enable floating-point support in printf
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float -u _scanf_float")

# Link the math library to support floating-point operations
target_link_libraries(${PROJECT_NAME} m)

# Allow GCC extensions (binary literals, etc.)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")

# Convert output to hex and binary
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
)

# Convert to bin file -> add conditional check?
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.bin
)

# # Add a custom command to run after the target is built
# message(STATUS "Building on host system: ${CMAKE_HOST_SYSTEM_NAME}")
# # Windows:
# if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
# add_custom_command(
#     TARGET ${CMAKE_PROJECT_NAME}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E echo "Starting DFU upload..."
#     COMMAND ../../dfu-util/dfu-util -a 0 -i 0 -s 0x08000000:leave -D ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
#     COMMENT "Uploading firmware using dfu-util"
# )
# else()
#      add_custom_command(
#      TARGET ${CMAKE_PROJECT_NAME}
#      POST_BUILD
#      COMMAND ${CMAKE_COMMAND} -E echo "Starting DFU upload..."
#      COMMAND dfu-util -a 0 -i 0 -s 0x08000000:leave -D ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
#      COMMENT "Uploading firmware using dfu-util"
#  )
# endif ()


