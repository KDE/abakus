PROJECT(abakus)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
FIND_PACKAGE(KDE4 REQUIRED)

include(KDE4Defaults)
include(CheckCXXSourceCompiles)

# Check for libmpfr and libgmp, and verify existence of _mpfr_struct
set(CMAKE_REQUIRED_LIBRARIES gmp mpfr)
check_cxx_source_compiles(
    "#include <mpfr.h>

    int main() {
    mpfr_t a;
    mpfr_ptr ptr;
    __mpfr_struct debug;

    mpfr_init(a);
    return 0;
    }
    "
    HAVE_MPFR
    )

INCLUDE_DIRECTORIES(${KDE4_INCLUDES} ${QT4_INCLUDES})
ADD_DEFINITIONS(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(doc)