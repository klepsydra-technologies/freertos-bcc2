
## [main]

# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 2.8.11)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
#project(ModernCMakeExample VERSION 1.0 LANGUAGES CXX)
project(ModernCMakeExample)
enable_language(ASM)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)

SET(ARCH_FLAGS "-mcpu=leon3 -qbsp=leon3 -std=gnu++11 -pedantic -Wall -O3")
SET(CMAKE_C_FLAGS "${ARCH_FLAGS}")
SET(CMAKE_CXX_FLAGS "${ARCH_FLAGS}")
#SET(CMAKE_EXE_LINKER_FLAGS "--entry=__wrap_start")

# Find packages go here.

# You should usually split this into folders, but this is a simple example

# This is a "default" library, and will match the *** variable setting.
# Other common choices are STATIC, SHARED, and MODULE
# Including header files here helps IDEs but is not required.
# Output libname matches target name, with the usual extensions on your system
#add_library(MyLibExample simple_lib.cpp simple_lib.hpp)

# Link each target with other targets or add options, etc.
include_directories(
    /opt/FreeRTOSv10.3.1-bcc2/include/freertos
    include
)
link_directories(/opt/FreeRTOSv10.3.1-bcc2/lib)

add_definitions(-D_GCC_MULTITHREAD_FREERTOS_ENABLE)

add_library(freertos-cpp11 STATIC
  lib/condition_variable.cpp
  lib/freertos_time.cpp
  lib/gthr_key.cpp
  lib/thread.cpp
  lib/future.cc
  lib/mutex.cc
)

# Adding something we can run - Output name matches target name
#add_executable(rtems-hello src/rtems-wrap_start.S src/rtems-hello.cpp)
add_executable(posix_demo
    main.c
    rtems-hello.cpp
)

# Make sure you link your targets with this command. It can also link libraries and
# even flags, so linking a target that does not exist will not give a configure-time error.
target_link_libraries(posix_demo PRIVATE freertos drv freertos-cpp11)

## [main]

# This part is so the Modern CMake book can verify this example builds. For your code,
# you'll probably want tests too
#enable_testing()
#add_test(NAME MyExample COMMAND MyExample)

set(CMAKE_INSTALL_PREFIX "/opt/FreeRTOSv10.3.1-bcc2")
set(INSTALL_LIB "lib")
set(INSTALL_INCLUDE "include/cpp11_gcc")

install(
    TARGETS freertos-cpp11
    DESTINATION ${INSTALL_LIB}
)

file(GLOB headers "include/*.h")
install(
    FILES ${headers}
    DESTINATION ${INSTALL_INCLUDE}
)

