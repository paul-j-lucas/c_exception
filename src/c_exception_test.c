/*
**      C Exception -- Exception Library for C
**      src/c_exception_test.c
**
**      Copyright (C) 2023-2024  Paul J. Lucas
**
**      This program is free software: you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation, either version 3 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// local
#include "config.h"                     /* must go first */
#include "c_exception.h"
#include "unit_test.h"

// standard
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

///////////////////////////////////////////////////////////////////////////////

// extern variables
char const       *me;

// local variables
static unsigned   test_failures;

////////// local functions ////////////////////////////////////////////////////

#define TEST_XID_ANY  0x0100
#define TEST_XID_01   0x0101
#define TEST_XID_02   0x0102

static bool test_no_throw( void ) {
  TEST_FN_BEGIN();
  unsigned n_try = 0, n_catch = 0, n_finally = 0;
  cx_try {
    ++n_try;
  }
  cx_catch( TEST_XID_01 ) {
    ++n_catch;
  }
  cx_finally {
    ++n_finally;
    TEST( cx_current_exception() == NULL );
  }
  TEST( n_try == 1 );
  TEST( n_catch == 0 );
  TEST( n_finally == 1 );
  TEST( cx_current_exception() == NULL );
  TEST_FN_END();
}

static bool test_throw_catch_1( void ) {
  TEST_FN_BEGIN();
  unsigned volatile n_try = 0;
  unsigned n_catch_1 = 0, n_catch_2 = 0, n_finally = 0;
  cx_try {
    ++n_try;
    cx_throw( TEST_XID_01 );
  }
  cx_catch( TEST_XID_01 ) {
    ++n_catch_1;
    TEST( cx_current_exception() != NULL );
  }
  cx_catch( TEST_XID_02 ) {
    ++n_catch_2;
  }
  cx_finally {
    ++n_finally;
    TEST( cx_current_exception() != NULL );
  }
  TEST( n_try == 1 );
  TEST( n_catch_1 == 1 );
  TEST( n_catch_2 == 0 );
  TEST( n_finally == 1 );
  TEST( cx_current_exception() == NULL );
  TEST_FN_END();
}

static bool test_throw_catch_2( void ) {
  TEST_FN_BEGIN();
  unsigned volatile n_try = 0;
  unsigned n_catch_1 = 0, n_catch_2 = 0, n_finally = 0;
  cx_try {
    ++n_try;
    cx_throw( TEST_XID_02 );
  }
  cx_catch( TEST_XID_01 ) {
    ++n_catch_1;
  }
  cx_catch( TEST_XID_02 ) {
    ++n_catch_2;
    TEST( cx_current_exception() != NULL );
  }
  cx_finally {
    ++n_finally;
    TEST( cx_current_exception() != NULL );
  }
  TEST( n_try == 1 );
  TEST( n_catch_1 == 0 );
  TEST( n_catch_2 == 1 );
  TEST( n_finally == 1 );
  TEST( cx_current_exception() == NULL );
  TEST_FN_END();
}

static bool test_throw_catch_all( void ) {
  TEST_FN_BEGIN();
  unsigned volatile n_try = 0;
  unsigned n_catch = 0, n_finally = 0;
  cx_try {
    ++n_try;
    cx_throw( TEST_XID_01 );
  }
  cx_catch() {
    ++n_catch;
  }
  cx_finally {
    ++n_finally;
    TEST( cx_current_exception() != NULL );
  }
  TEST( n_try == 1 );
  TEST( n_catch == 1 );
  TEST( n_finally == 1 );
  TEST( cx_current_exception() == NULL );
  TEST_FN_END();
}

static void test_throw_from_a_called_function_function( int xid ) {
  cx_throw( xid );
}

static bool test_throw_from_a_called_function( void ) {
  TEST_FN_BEGIN();
  unsigned volatile n_try = 0;
  unsigned n_catch = 0, n_finally = 0;
  cx_try {
    ++n_try;
    test_throw_from_a_called_function_function( TEST_XID_01 );
  }
  cx_catch( TEST_XID_01 ) {
    ++n_catch;
    TEST( cx_current_exception() != NULL );
  }
  cx_finally {
    ++n_finally;
    TEST( cx_current_exception() != NULL );
  }
  TEST( n_try == 1 );
  TEST( n_catch == 1 );
  TEST( n_finally == 1 );
  TEST( cx_current_exception() == NULL );
  TEST_FN_END();
}

static bool test_xid_matcher( int thrown_xid, int catch_xid ) {
  if ( (catch_xid & 0x00FF) == 0x00 )
    thrown_xid &= 0xFF00;
  return thrown_xid == catch_xid;
}

static bool test_custom_xid_matcher( void ) {
  TEST_FN_BEGIN();
  cx_xid_matcher_t prev = cx_set_xid_matcher( &test_xid_matcher );
  unsigned volatile n_try = 0;
  unsigned n_catch = 0;
  cx_try {
    ++n_try;
    cx_throw( TEST_XID_01 );
  }
  cx_catch( TEST_XID_ANY ) {
    ++n_catch;
  }
  cx_set_xid_matcher( prev );
  TEST( n_try == 1 );
  TEST( n_catch == 1 );
  TEST( cx_current_exception() == NULL );
  TEST_FN_END();
}

static bool test_throw_from_nested_catch( void ) {
  TEST_FN_BEGIN();
  unsigned volatile n_inner_try = 0, n_outer_try = 0;
  unsigned volatile n_inner_catch = 0, n_inner_finally = 0;
  unsigned n_outer_catch = 0, n_outer_finally = 0;
  cx_try {
    ++n_outer_try;
    cx_try {
      ++n_inner_try;
      cx_throw( TEST_XID_01 );
    }
    cx_catch( TEST_XID_01 ) {
      ++n_inner_catch;
      cx_throw( TEST_XID_02 );
    }
    cx_finally {
      ++n_inner_finally;
    }
  }
  cx_catch( TEST_XID_02 ) {
    ++n_outer_catch;
  }
  cx_finally {
    ++n_outer_finally;
  }
  TEST( n_inner_try == 1 );
  TEST( n_inner_catch == 1 );
  TEST( n_inner_finally == 1 );
  TEST( n_outer_try == 1 );
  TEST( n_outer_catch == 1 );
  TEST( n_outer_finally == 1 );
  TEST( cx_current_exception() == NULL );
  TEST_FN_END();
}

static bool test_rethrow_in_catch( void ) {
  TEST_FN_BEGIN();
  unsigned volatile n_inner_try = 0, n_outer_try = 0;
  unsigned volatile n_inner_catch = 0, n_inner_finally = 0;
  unsigned n_outer_catch = 0, n_outer_finally = 0;
  cx_try {
    ++n_outer_try;
    cx_try {
      ++n_inner_try;
      cx_throw( TEST_XID_01 );
    }
    cx_catch( TEST_XID_01 ) {
      ++n_inner_catch;
      cx_throw();
    }
    cx_finally {
      ++n_inner_finally;
    }
  }
  cx_catch( TEST_XID_01 ) {
    ++n_outer_catch;
  }
  cx_finally {
    ++n_outer_finally;
  }
  TEST( n_inner_try == 1 );
  TEST( n_inner_catch == 1 );
  TEST( n_inner_finally == 1 );
  TEST( n_outer_try == 1 );
  TEST( n_outer_catch == 1 );
  TEST( n_outer_finally == 1 );
  TEST( cx_current_exception() == NULL );
  TEST_FN_END();
}

static bool test_throw_with_user_data( void ) {
  TEST_FN_BEGIN();
  unsigned volatile n_try = 0;
  unsigned n_catch = 0;
  int user_data = 0;
  cx_try {
    ++n_try;
    user_data = 42;
    cx_throw( TEST_XID_01, &user_data );
  }
  cx_catch( TEST_XID_01 ) {
    ++n_catch;
    int *const pi = cx_user_data();
    if ( TEST( pi != NULL ) )
      TEST( *pi == 42 );
  }
  TEST( n_try == 1 );
  TEST( n_catch == 1 );
  TEST_FN_END();
}

int main( int argc, char const *argv[] ) {
  (void)argc;
  me = argv[0];

  test_no_throw();
  test_throw_catch_1();
  test_throw_catch_2();
  test_throw_catch_all();
  test_throw_from_a_called_function();
  test_custom_xid_matcher();
  test_throw_from_nested_catch();
  test_rethrow_in_catch();
  test_throw_with_user_data();

  printf( "%u failures\n", test_failures );
  exit( test_failures > 0 ? EX_SOFTWARE : EX_OK );
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
