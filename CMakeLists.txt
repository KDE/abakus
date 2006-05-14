find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

# Find bison, flex, gnu mp, mpfr here

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
link_directories(${KDE4_LIB_DIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(HAVE_MPFR 1)

configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})
add_subdirectory(src)