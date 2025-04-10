/**
 * Adds debugf for STM32.
 *
 * Add to CMakeList.txt:

# Enable hardware floating-point support in the compiler (if applicable)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Add the linker flag to enable floating-point support in printf
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u _printf_float -u _scanf_float")


# Link the math library to support floating-point operations
target_link_libraries(${PROJECT_NAME} m)

# Allow GCC extensions (binary literals, etc.)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")

* Change:

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
*

add to Src\usbd_cdc_if.c
#include "debugf_vcp.h"

 */

#ifndef DEBUGF_VCP_H
#define DEBUGF_VCP_H

void debugf(const char *__restrict format, ...);

void console_check();

void jump_to_dfu_bootloader();

#endif