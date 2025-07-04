# Process this file with autoconf to produce a configure script.

##
#       C Exception -- Exception Library for C
#       configure.ac
#
#       Copyright (C) 2023-2024  Paul J. Lucas
#
#       This program is free software: you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation, either version 3 of the License, or
#       (at your option) any later version.
#
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#
#       You should have received a copy of the GNU General Public License
#       along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

AC_PREREQ([2.69])
AC_INIT([c_exception],[1.1.1],[https://github.com/paul-j-lucas/c_exception/issues],[],[https://github.com/paul-j-lucas/c_exception])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_LANG(C)
AC_PROG_CC
AS_IF([test "x$ac_cv_prog_cc_c11" = xno], [
  AC_MSG_ERROR([a C11 compiler is required to compile $PACKAGE_NAME])
])
gl_EARLY

AC_SUBST([C_EXCEPTION_CFLAGS])
C_EXCEPTION_CFLAGS="-D_ISOC11_SOURCE"

AM_PROG_AR
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([sysexits.h])
AC_HEADER_ASSERT
AC_HEADER_STDBOOL
gl_INIT

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_REALLOC

# Testing feature: Address Sanitizer (ASan)
AC_ARG_ENABLE([asan],
  AS_HELP_STRING([--enable-asan],
    [enable Address Sanitizer for debugging]),
  [],
  [enable_asan=no]
)
AS_IF([test "x$enable_asan" = xyes], [
  AC_DEFINE([ENABLE_ASAN], [1], [Define to 1 if Address Sanitizer is enabled.])
])

# Testing feature: Memory Sanitizer (MSan)
AC_ARG_ENABLE([msan],
  AS_HELP_STRING([--enable-msan],
    [enable Memory Sanitizer for debugging]),
  [],
  [enable_msan=no]
)
AS_IF([test "x$enable_msan" = xyes], [
  AC_DEFINE([ENABLE_MSAN], [1],
    [Define to 1 if Memory Sanitizer is enabled.])
])

# Testing feature: Undefined Behavior Sanitizer (UBSan)
AC_ARG_ENABLE([ubsan],
  AS_HELP_STRING([--enable-ubsan],
    [enable Undefined Bahavior Sanitizer for debugging]),
  [],
  [enable_ubsan=no]
)
AS_IF([test "x$enable_ubsan" = xyes], [
  AC_DEFINE([ENABLE_UBSAN], [1],
    [Define to 1 if Undefined Bahavior Sanitizer is enabled.])
])

# Makefile conditionals.
AM_CONDITIONAL([ENABLE_ASAN],         [test "x$enable_asan"         = xyes])
AM_CONDITIONAL([ENABLE_MSAN],         [test "x$enable_msan"         = xyes])
AM_CONDITIONAL([ENABLE_UBSAN],        [test "x$enable_ubsan"        = xyes])

# Miscellaneous.
AX_C___ATTRIBUTE__

# Compiler warnings.
AX_CFLAGS_WARN_ALL([C_EXCEPTION_CFLAGS])
AX_CHECK_COMPILE_FLAG([-Wcast-align], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wcast-align"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wcast-function-type], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wcast-function-type"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wcomma], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wcomma"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wconditional-type-mismatch], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wconditional-type-mismatch"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wconditional-uninitialized], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wconditional-uninitialized"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wconversion], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wconversion"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wno-dangling-else], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wno-dangling-else"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wduplicate-enum], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wduplicate-enum"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wembedded-directive], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wembedded-directive"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wempty-body], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wempty-body"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wenum-enum-conversion], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wenum-enum-conversion"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wenum-float-conversion], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wenum-float-conversion"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wextra], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wextra"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wfloat-equal], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wfloat-equal"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wfor-loop-analysis], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wfor-loop-analysis"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wformat-nonliteral], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wformat-nonliteral"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wformat-signedness], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wformat-signedness"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wformat-type-confusion], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wformat-type-confusion"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wformat=2], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wformat=2"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Widiomatic-parentheses], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Widiomatic-parentheses"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wimplicit-fallthrough], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wimplicit-fallthrough"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wlogical-op-parentheses], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wlogical-op-parentheses"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wmisleading-indentation], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wmisleading-indentation"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wnewline-eof], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wnewline-eof"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wredundant-decls], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wredundant-decls"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wreserved-identifier], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wreserved-identifier"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wshift-sign-overflow], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wshift-sign-overflow"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wshorten-64-to-32], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wshorten-64-to-32"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wsign-compare], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wsign-compare"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wsign-conversion], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wsign-conversion"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wsometimes-uninitialized], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wsometimes-uninitialized"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wstring-conversion], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wstring-conversion"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wtautological-compare], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wtautological-compare"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wtautological-type-limit-compare], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wtautological-type-limit-compare"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wundef], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wundef"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wuninitialized], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wuninitialized"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wunreachable-code-break], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wunreachable-code-break"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wunreachable-code-return], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wunreachable-code-return"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wunreachable-code], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wunreachable-code"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wunused], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wunused"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wwrite-strings], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wwrite-strings"], [], [-Werror])
AX_CHECK_COMPILE_FLAG([-Wzero-as-null-pointer-constant], [C_EXCEPTION_CFLAGS="$C_EXCEPTION_CFLAGS -Wzero-as-null-pointer-constant"], [], [-Werror])

# Generate files.
AH_TOP([#ifndef c_exception_config_H
#define c_exception_config_H])
AH_BOTTOM([#endif /* c_exception_config_H */])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([
  Makefile
  lib/Makefile
  src/Makefile
])
AC_OUTPUT

# vim:set et sw=2 ts=2:
