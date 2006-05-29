# - Try to find the mpfr directory library
# Once done this will define
#
#  MPFR_FOUND - system has MPFR
#  MPFR_INCLUDE_DIR - the MPFR include directory
#  MPFR_LIBRARIES - The libraries needed to use MPFR

include(CheckCXXSourceCompiles)

if(MPFR_INCLUDE_DIR AND MPFR_LIBRARIES AND MPFR_COMPILES)
   set(MPFR_FIND_QUIETLY TRUE)
endif(MPFR_INCLUDE_DIR AND MPFR_LIBRARIES AND MPFR_COMPILES)

FIND_PATH(MPFR_INCLUDE_DIR mpfr.h
   /usr/include
   /usr/local/include
)

FIND_LIBRARY(MPFR_LIBRARY mpfr
   PATHS
   /usr/lib
   /usr/local/lib
)

# We actually need gmp too.
FIND_LIBRARY(GMP_LIBRARY gmp
   PATHS
   /usr/lib
   /usr/local/lib
)

# We have found MPFR and gmp, make sure our sample compiles, letting us know
# that it is actually compatible with abakus.
if(MPFR_INCLUDE_DIR AND MPFR_LIBRARY AND GMP_LIBRARY)
   set(MPFR_LIBRARIES ${MPFR_LIBRARY} ${GMP_LIBRARY})

   # MPFR_LIBRARIES has paths, apparently not what we want here.
   set(CMAKE_REQUIRED_LIBRARIES mpfr gmp) 
   set(CMAKE_REQUIRED_INCLUDES ${MPFR_INCLUDE_DIR})
   
   check_cxx_source_compiles("
#include <mpfr.h>

int main() {
   mpfr_t a;
   mpfr_ptr ptr;
   __mpfr_struct debug;

   mpfr_init(a);
   (void) ptr;
   (void) debug;
   return 0;
}
" MPFR_COMPILES)

   if(MPFR_COMPILES)
      set(MPFR_FOUND TRUE)
      if(NOT MPFR_FIND_QUIETLY)
         message(STATUS "Found MPFR: ${MPFR_LIBRARIES}")
      endif(NOT MPFR_FIND_QUIETLY)
   else(MPFR_COMPILES)
      message(STATUS "MPFR library was found but appears to be incompatible.")
      message(STATUS "Using built-in high precision routines.")
   endif(MPFR_COMPILES)
endif(MPFR_INCLUDE_DIR AND MPFR_LIBRARY AND GMP_LIBRARY)

MARK_AS_ADVANCED(MPFR_INCLUDE_DIR MPFR_LIBRARY GMP_LIBRARY MPFR_COMPILES)
