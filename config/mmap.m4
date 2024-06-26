dnl ----------------------------------------------------------------------
dnl This whole bit snagged from gcc

dnl
dnl mmap(2) blacklisting.  Some platforms provide the mmap library routine
dnl but don't support all of the features we need from it.
dnl
AC_DEFUN([GCC_AC_FUNC_MMAP_BLACKLIST],
[
AC_CHECK_HEADER([sys/mman.h],
		[gcc_header_sys_mman_h=yes], [gcc_header_sys_mman_h=no])
AC_CHECK_FUNC([mmap], [gcc_func_mmap=yes], [gcc_func_mmap=no])
if test "$gcc_header_sys_mman_h" != yes \
 || test "$gcc_func_mmap" != yes; then
   gcc_cv_func_mmap_file=no
   gcc_cv_func_mmap_dev_zero=no
   gcc_cv_func_mmap_anon=no
else
   AC_CACHE_CHECK([whether read-only mmap of a plain file works], 
  gcc_cv_func_mmap_file,
  [# Add a system to this blacklist if 
   # mmap(0, stat_size, PROT_READ, MAP_PRIVATE, fd, 0) doesn't return a
   # memory area containing the same data that you'd get if you applied
   # read() to the same fd.  The only system known to have a problem here
   # is VMS, where text files have record structure.
   case "$host_os" in
     *vms* | ultrix*)
        gcc_cv_func_mmap_file=no ;;
     *)
        gcc_cv_func_mmap_file=yes;;
   esac])
   AC_CACHE_CHECK([whether mmap from /dev/zero works],
  gcc_cv_func_mmap_dev_zero,
  [# Add a system to this blacklist if it has mmap() but /dev/zero
   # does not exist, or if mmapping /dev/zero does not give anonymous
   # zeroed pages with both the following properties:
   # 1. If you map N consecutive pages in with one call, and then
   #    unmap any subset of those pages, the pages that were not
   #    explicitly unmapped remain accessible.
   # 2. If you map two adjacent blocks of memory and then unmap them
   #    both at once, they must both go away.
   # Systems known to be in this category are Windows (all variants),
   # VMS, and Darwin.
   case "$host_os" in
     *vms* | cygwin* | pe | mingw* | darwin* | ultrix* | hpux10* | hpux11.00)
        gcc_cv_func_mmap_dev_zero=no ;;
     *)
        gcc_cv_func_mmap_dev_zero=yes;;
   esac])

   # Unlike /dev/zero, the MAP_ANON(YMOUS) defines can be probed for.
   AC_CACHE_CHECK([for MAP_ANON(YMOUS)], gcc_cv_decl_map_anon,
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
[#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif
],
[int n = MAP_ANONYMOUS;])],
    gcc_cv_decl_map_anon=yes,
    gcc_cv_decl_map_anon=no)])

   if test $gcc_cv_decl_map_anon = no; then
     gcc_cv_func_mmap_anon=no
   else
     AC_CACHE_CHECK([whether mmap with MAP_ANON(YMOUS) works],
     gcc_cv_func_mmap_anon,
  [# Add a system to this blacklist if it has mmap() and MAP_ANON or
   # MAP_ANONYMOUS, but using mmap(..., MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)
   # doesn't give anonymous zeroed pages with the same properties listed
   # above for use of /dev/zero.
   # Systems known to be in this category are Windows, VMS, and SCO Unix.
   case "$host_os" in
     *vms* | cygwin* | pe | mingw* | sco* | udk* )
        gcc_cv_func_mmap_anon=no ;;
     *)
        gcc_cv_func_mmap_anon=yes;;
   esac])
   fi
fi

if test $gcc_cv_func_mmap_file = yes; then
  AC_DEFINE(HAVE_MMAP_FILE, 1,
	    [Define if read-only mmap of a plain file works.])
fi
if test $gcc_cv_func_mmap_dev_zero = yes; then
  AC_DEFINE(HAVE_MMAP_DEV_ZERO, 1,
	    [Define if mmap of /dev/zero works.])
fi
if test $gcc_cv_func_mmap_anon = yes; then
  AC_DEFINE(HAVE_MMAP_ANON, 1,
	    [Define if mmap with MAP_ANON(YMOUS) works.])
fi
])

dnl
dnl Avoid the sanitizer run-time memory leak failure in the mmap configure
dnl test.  This should be removed when autoconf with commit:
dnl
dnl commit 09b6e78d1592ce10fdc975025d699ee41444aa3f
dnl Author: Paul Eggert <eggert@cs.ucla.edu>
dnl Date:   Fri Feb 5 21:06:20 2016 -0800
dnl Fix memory leak in AC_FUNC_MMAP
dnl
dnl * lib/autoconf/functions.m4 (AC_FUNC_MMAP): Fix memory leak
dnl in test case, found by configuring with gcc -fsanitize=address.
dnl
dnl is in use.
dnl
AC_DEFUN([GCC_AC_FUNC_MMAP],
  save_ASAN_OPTIONS="$ASAN_OPTIONS"
  ASAN_OPTIONS=detect_leaks=0
  export ASAN_OPTIONS
  m4_defn([AC_FUNC_MMAP])
  ASAN_OPTIONS="$save_ASAN_OPTIONS"
)
