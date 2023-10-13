##
#       C Exception -- Exception Library for C
#	src/Makefile.am
#
#	Copyright (C) 2023  Paul J. Lucas
#
#	This program is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

LIBRARIES =	c_exception
check_PROGRAMS=	c_exception_test

AM_CFLAGS =	$(C_EXCEPTION_CFLAGS)

if ENABLE_ASAN
AM_CFLAGS +=	-fsanitize=address -fno-omit-frame-pointer
endif

if ENABLE_MSAN
AM_CFLAGS +=	-fsanitize=memory -fno-omit-frame-pointer
endif

if ENABLE_UBSAN
AM_CFLAGS +=	-fsanitize=undefined
endif

AM_CPPFLAGS =	-I$(top_srcdir)/lib -I$(top_builddir)/lib
LDADD =		$(top_builddir)/lib/libgnu.a

c_exception_SOURCES = \
		c_exception.c c_exception.h

c_exception_test_SOURCES = \
		c_exeption.c c_exeption.h \
		c_exception_test.c \
		unit_test.h

TESTS =		$(check_PROGRAMS)

# vim:set noet sw=8 ts=8:
