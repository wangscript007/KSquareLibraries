#if  !defined(_DEBUG)
#define  _NO_MEMORY_LEAK_CHECK_
#endif

//#ifndef  WIN32
//#define WIN32
//#endif


#if  defined(WIN32)
  #define  KKOS_WINDOWS
  #if  !defined(DOXYGEN)
    #include <windows.h>
  #endif
#endif

#if  defined(WIN64)
#define  KKOS_WINDOWS
#endif

#if  defined(_MSC_VER)
  #if _MSC_VER >= 1400
    // We are using VS2005 or later; so we want to use the secure functions.
    #define  USE_SECURE_FUNCS
  #endif
#else
  // Since we are using Microsoft's memory leak detection and we are not using a MS compiler can not do memory leak check.
  #define  _NO_MEMORY_LEAK_CHECK_
#endif

#if  !defined(_NO_MEMORY_LEAK_CHECK_)
  //  _NO_MEMORY_LEAK_CHECK_  Put there by Kurt so that we can exclude
  //  this code.  Specifically used when doing the DEBUG Multi threaded
  // environment.
  #if  !defined(_WINDOWS)
    #if  defined(WIN32)  
      #include <windows.h>
      #if  defined(_DEBUG)
        #define _CRTDBG_MAP_ALLOC
        #include <stdlib.h>
        #define  STDLIB_INCLUDED
        #include <crtdbg.h>
        #define MYDEBUG_NEW   new(_NORMAL_BLOCK, __FILE__, __LINE__)
      #else
        #if  !defined(_WINDOWS)
          #include <stdlib.h>
          #define  STDLIB_INCLUDED
        #endif
      #endif
    #else
      #define _ASSERTE(xx)
    #endif
  #endif
#endif


#if  !defined(STDLIB_INCLUDED)
#include  <stdlib.h>
#include  <stdio.h>
#define  STDLIB_INCLUDED
#endif


#if  defined(KKOS_WINDOWS)

#define  DisableConversionWarning (alpha)  \
#pragma warning( push )                    \
#pragma warning( disable : 4101)

#define  RestoreConversionWarning (alpha)  \
#pragma warning( pop ) 
#else

#define  DisableConversionWarning (alpha)        \
#pragma GCC diagnostic push                      \
#pragma GCC diagnostic ignored "-Wconversion"

#define  RestoreConversionWarning (alpha)  \
#pragma GCC diagnostic pop 

#endif

