/*
    MACRO/map.h -- Preprocessor meta-macro to apply a macro to list entries

    The contents of this file are rather ugly, but they provide two operations
    that are extremely useful for code generation, and which work reliably on a
    variety of different compilers (in particular, I've tested GCC with both
    -std=c++11/14/17 and -std=gnu++11/14/17, which have some macro-related
    differences, Clang, and MSVC.).

    The first meta-macro

        MACRO_MAP(MY_MACRO, a, b, c)

    expands to

        MY_MACRO(a) MY_MACRO(b) MY_MACRO(c)

    The second

        MACRO_MAPC(MY_MACRO, a, b, c)

    expands to

        MY_MACRO(a), MY_MACRO(b), MY_MACRO(c)

    (note the extra commans between arguments). The implementation supports a
    maximum of 32 arguments, which ought to be enough for everyone.

    The implementation is based on tricks proposed by Laurent Deniau and
    Joshua Ryan (https://stackoverflow.com/questions/6707148).

    Dr.Jit is a C++ template library for efficient vectorization and
    differentiation of numerical kernels on modern processor architectures.

    Copyright (c) 2021 Wenzel Jakob <wenzel.jakob@epfl.ch>

    All rights reserved. Use of this source code is governed by a BSD-style
    license that can be found in the LICENSE file.
*/

#pragma once

// Macro to compute the size of __VA_ARGS__ (including 0, works on Clang/GCC/MSVC)
#define MACRO_EVAL(x) x

#define MACRO_VA_SIZE_3(_, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, \
        _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, \
        _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
        _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, \
        _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N

#define MACRO_VA_SIZE_2(...) MACRO_EVAL(MACRO_VA_SIZE_3(__VA_ARGS__))

#if defined(__GNUC__) && !defined(__clang__)
#  define MACRO_VA_SIZE_1(...) _ __VA_OPT__(,) __VA_ARGS__ , \
    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, \
    44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, \
    25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, \
    5, 4, 3, 2, 1, 0
#else
#  define MACRO_VA_SIZE_1(...) _, ##__VA_ARGS__ , \
    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, \
    44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, \
    25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, \
    5, 4, 3, 2, 1, 0
#endif

#define MACRO_VA_SIZE(...) MACRO_VA_SIZE_2(MACRO_VA_SIZE_1(__VA_ARGS__))


#define MACRO_MAPN_0(...)
#define MACRO_MAPC_0(...)
#define MACRO_MAPN_1(Z,a) Z(a)
#define MACRO_MAPC_1(Z,a) Z(a)
#define MACRO_MAPN_2(Z,a,b) Z(a)Z(b)
#define MACRO_MAPC_2(Z,a,b) Z(a),Z(b)
#define MACRO_MAPN_3(Z,a,b,c) Z(a)Z(b)Z(c)
#define MACRO_MAPC_3(Z,a,b,c) Z(a),Z(b),Z(c)
#define MACRO_MAPN_4(Z,a,b,c,d) Z(a)Z(b)Z(c)Z(d)
#define MACRO_MAPC_4(Z,a,b,c,d) Z(a),Z(b),Z(c),Z(d)
#define MACRO_MAPN_5(Z,a,b,c,d,e) Z(a)Z(b)Z(c)Z(d)Z(e)
#define MACRO_MAPC_5(Z,a,b,c,d,e) Z(a),Z(b),Z(c),Z(d),Z(e)
#define MACRO_MAPN_6(Z,a,b,c,d,e,f) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)
#define MACRO_MAPC_6(Z,a,b,c,d,e,f) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f)
#define MACRO_MAPN_7(Z,a,b,c,d,e,f,g) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)
#define MACRO_MAPC_7(Z,a,b,c,d,e,f,g) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g)
#define MACRO_MAPN_8(Z,a,b,c,d,e,f,g,h) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)
#define MACRO_MAPC_8(Z,a,b,c,d,e,f,g,h) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h)
#define MACRO_MAPN_9(Z,a,b,c,d,e,f,g,h,i) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)
#define MACRO_MAPC_9(Z,a,b,c,d,e,f,g,h,i) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i)
#define MACRO_MAPN_10(Z,a,b,c,d,e,f,g,h,i,j) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)
#define MACRO_MAPC_10(Z,a,b,c,d,e,f,g,h,i,j) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j)
#define MACRO_MAPN_11(Z,a,b,c,d,e,f,g,h,i,j,k) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)
#define MACRO_MAPC_11(Z,a,b,c,d,e,f,g,h,i,j,k) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k)
#define MACRO_MAPN_12(Z,a,b,c,d,e,f,g,h,i,j,k,l) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)
#define MACRO_MAPC_12(Z,a,b,c,d,e,f,g,h,i,j,k,l) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l)
#define MACRO_MAPN_13(Z,a,b,c,d,e,f,g,h,i,j,k,l,m) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)
#define MACRO_MAPC_13(Z,a,b,c,d,e,f,g,h,i,j,k,l,m) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m)
#define MACRO_MAPN_14(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)
#define MACRO_MAPC_14(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n)
#define MACRO_MAPN_15(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)
#define MACRO_MAPC_15(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o)
#define MACRO_MAPN_16(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)
#define MACRO_MAPC_16(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p)
#define MACRO_MAPN_17(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)
#define MACRO_MAPC_17(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q)
#define MACRO_MAPN_18(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)
#define MACRO_MAPC_18(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r)
#define MACRO_MAPN_19(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)
#define MACRO_MAPC_19(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s)
#define MACRO_MAPN_20(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)
#define MACRO_MAPC_20(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t)
#define MACRO_MAPN_21(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)
#define MACRO_MAPC_21(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u)
#define MACRO_MAPN_22(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)
#define MACRO_MAPC_22(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v)
#define MACRO_MAPN_23(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)
#define MACRO_MAPC_23(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w)
#define MACRO_MAPN_24(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)
#define MACRO_MAPC_24(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x)
#define MACRO_MAPN_25(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)
#define MACRO_MAPC_25(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y)
#define MACRO_MAPN_26(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)
#define MACRO_MAPC_26(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z)
#define MACRO_MAPN_27(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(A)
#define MACRO_MAPC_27(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(A)
#define MACRO_MAPN_28(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(A)Z(B)
#define MACRO_MAPC_28(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(A),Z(B)
#define MACRO_MAPN_29(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(A)Z(B)Z(C)
#define MACRO_MAPC_29(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(A),Z(B),Z(C)
#define MACRO_MAPN_30(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(A)Z(B)Z(C)Z(D)
#define MACRO_MAPC_30(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(A),Z(B),Z(C),Z(D)
#define MACRO_MAPN_31(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(A)Z(B)Z(C)Z(D)Z(E)
#define MACRO_MAPC_31(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D,E) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(A),Z(B),Z(C),Z(D),Z(E)
#define MACRO_MAPN_32(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)
#define MACRO_MAPC_32(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af)
#define MACRO_MAPN_33(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)
#define MACRO_MAPC_33(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag)
#define MACRO_MAPN_34(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)
#define MACRO_MAPC_34(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah)
#define MACRO_MAPN_35(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)
#define MACRO_MAPC_35(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai)
#define MACRO_MAPN_36(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)
#define MACRO_MAPC_36(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj)
#define MACRO_MAPN_37(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)
#define MACRO_MAPC_37(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak)
#define MACRO_MAPN_38(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)
#define MACRO_MAPC_38(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al)
#define MACRO_MAPN_39(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)
#define MACRO_MAPC_39(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am)
#define MACRO_MAPN_40(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)
#define MACRO_MAPC_40(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an)
#define MACRO_MAPN_41(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)
#define MACRO_MAPC_41(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao)
#define MACRO_MAPN_42(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)
#define MACRO_MAPC_42(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap)
#define MACRO_MAPN_43(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)
#define MACRO_MAPC_43(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq)
#define MACRO_MAPN_44(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)
#define MACRO_MAPC_44(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar)
#define MACRO_MAPN_45(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)
#define MACRO_MAPC_45(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as)
#define MACRO_MAPN_46(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)
#define MACRO_MAPC_46(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at)
#define MACRO_MAPN_47(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)
#define MACRO_MAPC_47(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au)
#define MACRO_MAPN_48(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)
#define MACRO_MAPC_48(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av)
#define MACRO_MAPN_49(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)
#define MACRO_MAPC_49(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw)
#define MACRO_MAPN_50(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)
#define MACRO_MAPC_50(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax)
#define MACRO_MAPN_51(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)
#define MACRO_MAPC_51(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay)
#define MACRO_MAPN_52(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)
#define MACRO_MAPC_52(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az)
#define MACRO_MAPN_53(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)
#define MACRO_MAPC_53(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba)
#define MACRO_MAPN_54(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)
#define MACRO_MAPC_54(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb)
#define MACRO_MAPN_55(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)
#define MACRO_MAPC_55(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc)
#define MACRO_MAPN_56(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)
#define MACRO_MAPC_56(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd)
#define MACRO_MAPN_57(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)Z(be)
#define MACRO_MAPC_57(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd),Z(be)
#define MACRO_MAPN_58(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)Z(be)Z(bf)
#define MACRO_MAPC_58(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd),Z(be),Z(bf)
#define MACRO_MAPN_59(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)Z(be)Z(bf)Z(bg)
#define MACRO_MAPC_59(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd),Z(be),Z(bf),Z(bg)
#define MACRO_MAPN_60(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)Z(be)Z(bf)Z(bg)Z(bh)
#define MACRO_MAPC_60(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd),Z(be),Z(bf),Z(bg),Z(bh)
#define MACRO_MAPN_61(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh,bi) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)Z(be)Z(bf)Z(bg)Z(bh)Z(bi)
#define MACRO_MAPC_61(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh,bi) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd),Z(be),Z(bf),Z(bg),Z(bh),Z(bi)
#define MACRO_MAPN_62(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh,bi,bj) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)Z(be)Z(bf)Z(bg)Z(bh)Z(bi)Z(bj)
#define MACRO_MAPC_62(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh,bi,bj) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd),Z(be),Z(bf),Z(bg),Z(bh),Z(bi),Z(bj)
#define MACRO_MAPN_63(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh,bi,bj,bk) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)Z(be)Z(bf)Z(bg)Z(bh)Z(bi)Z(bj)Z(bk)
#define MACRO_MAPC_63(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh,bi,bj,bk) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd),Z(be),Z(bf),Z(bg),Z(bh),Z(bi),Z(bj),Z(bk)
#define MACRO_MAPN_64(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh,bi,bj,bk,bl) Z(a)Z(b)Z(c)Z(d)Z(e)Z(f)Z(g)Z(h)Z(i)Z(j)Z(k)Z(l)Z(m)Z(n)Z(o)Z(p)Z(q)Z(r)Z(s)Z(t)Z(u)Z(v)Z(w)Z(x)Z(y)Z(z)Z(aa)Z(ab)Z(ac)Z(ad)Z(ae)Z(af)Z(ag)Z(ah)Z(ai)Z(aj)Z(ak)Z(al)Z(am)Z(an)Z(ao)Z(ap)Z(aq)Z(ar)Z(as)Z(at)Z(au)Z(av)Z(aw)Z(ax)Z(ay)Z(az)Z(ba)Z(bb)Z(bc)Z(bd)Z(be)Z(bf)Z(bg)Z(bh)Z(bi)Z(bj)Z(bk)Z(bl)
#define MACRO_MAPC_64(Z,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af,ag,ah,ai,aj,ak,al,am,an,ao,ap,aq,ar,as,at,au,av,aw,ax,ay,az,ba,bb,bc,bd,be,bf,bg,bh,bi,bj,bk,bl) Z(a),Z(b),Z(c),Z(d),Z(e),Z(f),Z(g),Z(h),Z(i),Z(j),Z(k),Z(l),Z(m),Z(n),Z(o),Z(p),Z(q),Z(r),Z(s),Z(t),Z(u),Z(v),Z(w),Z(x),Z(y),Z(z),Z(aa),Z(ab),Z(ac),Z(ad),Z(ae),Z(af),Z(ag),Z(ah),Z(ai),Z(aj),Z(ak),Z(al),Z(am),Z(an),Z(ao),Z(ap),Z(aq),Z(ar),Z(as),Z(at),Z(au),Z(av),Z(aw),Z(ax),Z(ay),Z(az),Z(ba),Z(bb),Z(bc),Z(bd),Z(be),Z(bf),Z(bg),Z(bh),Z(bi),Z(bj),Z(bk),Z(bl)

#define MACRO_CONCAT_(a,b) a ## b
#define MACRO_CONCAT(a,b) MACRO_CONCAT_(a,b)
#define MACRO_MAP_(M, Z, ...) MACRO_EVAL(M(Z, __VA_ARGS__))
#define MACRO_MAP(Z, ...)  MACRO_MAP_(MACRO_CONCAT(MACRO_MAPN_, MACRO_VA_SIZE(__VA_ARGS__)), Z, __VA_ARGS__)
#define MACRO_MAPC(Z, ...) MACRO_MAP_(MACRO_CONCAT(MACRO_MAPC_, MACRO_VA_SIZE(__VA_ARGS__)), Z, __VA_ARGS__)