#pragma once

#define USTC_CG_NAMESPACE_OPEN_SCOPE namespace USTC_CG{
#define USTC_CG_NAMESPACE_CLOSE_SCOPE } // namespace ustc_cg


// Modified from Mitsuba3
#if defined(_MSC_VER)
#  define USTC_CG_EXPORT   __declspec(dllexport)
#  define USTC_CG_IMPORT   __declspec(dllimport)
#  define USTC_CG_NOINLINE __declspec(noinline)
#  define USTC_CG_INLINE   __forceinline
#else
#  define USTC_CG_EXPORT    __attribute__ ((visibility("default")))
#  define USTC_CG_IMPORT
#  define USTC_CG_NOINLINE  __attribute__ ((noinline))
#  define USTC_CG_INLINE    __attribute__((always_inline)) inline
#endif

#define USTC_CG_MODULE_LIB    1
#define USTC_CG_MODULE_UI     2

#if USTC_CG_BUILD_MODULE == USTC_CG_MODULE_LIB
#  define USTC_CG_API USTC_CG_EXPORT
#  define USTC_CG_EXTERN extern
#else
#  define USTC_CG_API USTC_CG_IMPORT
#  if defined(_MSC_VER)
#    define USTC_CG_EXTERN
#  else
#    define USTC_CG_EXTERN extern
#  endif
#endif