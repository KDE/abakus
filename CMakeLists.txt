project(abakus)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(KDE4 REQUIRED)
find_package(KDeclarative REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

find_package(MPFR REQUIRED)
set_package_properties(MPFR PROPERTIES DESCRIPTION "MPFR provides a library for multiple-precision floating-point computations with correct rounding"
                       URL "http://www.mpfr.org"
                       TYPE REQUIRED
                      )

find_package(FLEX REQUIRED)
set_package_properties(FLEX PROPERTIES DESCRIPTION "Flex is a tool for generating scanners, which recognizes lexical patterns"
                       URL "http://flex.sourceforge.net"
                       TYPE REQUIRED
                      )

find_package(BISON REQUIRED)
set_package_properties(BISON PROPERTIES DESCRIPTION "Bison is a general-purpose parser generator"
                       URL "http://www.gnu.org/software/bison"
                       TYPE REQUIRED
                      )


include_directories(${MPFR_INCLUDE_DIR} ${QT4_INCLUDES} ${KDE4_INCLUDES} ${KDECLARATIVE_INCLUDE_DIR})

add_subdirectory(src)
add_subdirectory(pics)
add_subdirectory(doc)
