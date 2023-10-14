/*
**      C Exception -- Exception Library for C
**      src/c_exception.c
**
**      Copyright (C) 2023  Paul J. Lucas
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

/**
 * @file
 * Defines variables and functions to implement C++-like exception handling in
 * C.
 */

// local
#include "config.h"                     /* must go first */
#include "c_exception.h"

// standard
#include <assert.h>
#include <attribute.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#if   __STDC_VERSION__ >= 202311L
# define CX_IMPL_THREAD_LOCAL     thread_local
#elif __STDC_VERSION__ >= 201112L
# define CX_IMPL_THREAD_LOCAL     _Thread_local
#elif defined( _MSC_VER )
# define CX_IMPL_THREAD_LOCAL     __declspec( thread )
#elif defined( __GNUC__ )
# define CX_IMPL_THREAD_LOCAL     __thread
#else
# error "Don't know how to declare thread-local variables on this platform."
#endif

// local functions
_Noreturn
static void cx_default_terminate_handler( cx_exception_t const* );
static bool cx_default_xid_matcher( int, int );

/**
 * Current exception.
 */
static CX_IMPL_THREAD_LOCAL cx_exception_t cx_exception;

/**
 * Current terminate handler.
 */
static cx_terminate_handler_t cx_terminate_handler =
  &cx_default_terminate_handler;

/**
 * Linked list of open `try` blocks.
 */
static CX_IMPL_THREAD_LOCAL cx_impl_try_block_t *cx_try_block_head;

/**
 * Current exception matcher function.
 */
static cx_xid_matcher_t cx_xid_matcher = &cx_default_xid_matcher;

////////// local functions ////////////////////////////////////////////////////

/**
 * Default terminate handler.
 *
 * @param cex A pointer to a cx_exception object that has information about the
 * exception that was thrown.
 */
_Noreturn
static void cx_default_terminate_handler( cx_exception_t const *cex ) {
  assert( cex != NULL );
  fprintf( stderr,
    "%s:%d: unhandled exception %d (0x%X)\n",
    cex->file, cex->line,
    cex->thrown_xid, (unsigned)cex->thrown_xid
  );
  abort();
}

/**
 * Default exception matcher function.
 *
 * @param thrown_xid The thrown exception ID.
 * @param catch_xid The exception ID to match \a thrown_xid against.
 * @return Returns `true` only if \a thrown_xid equals \a catch_xid.
 */
static bool cx_default_xid_matcher( int thrown_xid, int catch_xid ) {
  return thrown_xid == catch_xid;
}

/**
 * Actually "throws" the current exception.
 */
_Noreturn
static void cx_do_throw( void ) {
  if ( cx_try_block_head == NULL )
    cx_terminate();
  cx_try_block_head->state = CX_IMPL_THROWN;
  cx_try_block_head->thrown_xid = cx_exception.thrown_xid;
  longjmp( cx_try_block_head->env, 1 );
}

////////// extern public functions ////////////////////////////////////////////

void cx_cancel_try( void ) {
  if ( cx_try_block_head != NULL )
    cx_try_block_head = cx_try_block_head->parent;
}

cx_terminate_handler_t cx_get_terminate( void ) {
  return cx_terminate_handler == &cx_default_terminate_handler ?
    NULL : cx_terminate_handler;
}

cx_xid_matcher_t cx_get_xid_matcher( void ) {
  return cx_xid_matcher == &cx_default_xid_matcher ? NULL : cx_xid_matcher;
}

cx_terminate_handler_t cx_set_terminate( cx_terminate_handler_t fn ) {
  cx_terminate_handler_t const rv = cx_terminate_handler;
  cx_terminate_handler = fn == NULL ? &cx_default_terminate_handler : fn;
  return rv;
}

cx_xid_matcher_t cx_set_xid_matcher( cx_xid_matcher_t fn ) {
  cx_xid_matcher_t const rv = cx_xid_matcher;
  cx_xid_matcher = fn == NULL ? &cx_default_xid_matcher : fn;
  return rv;
}

void cx_terminate( void ) {
  assert( cx_terminate_handler != NULL );
  (*cx_terminate_handler)( &cx_exception );
  unreachable();
}

////////// extern implementation functions ////////////////////////////////////

bool cx_impl_catch( int catch_xid, cx_impl_try_block_t *tb ) {
  assert( tb != NULL );
  assert( tb->state == CX_IMPL_THROWN );
  assert( cx_xid_matcher != NULL );
  if ( !(*cx_xid_matcher)( tb->thrown_xid, catch_xid ) )
    return false;
  tb->state = CX_IMPL_CAUGHT;
  return true;
}

bool cx_impl_catch_all( cx_impl_try_block_t *tb ) {
  assert( tb != NULL );
  assert( tb->state == CX_IMPL_THROWN );
  tb->state = CX_IMPL_CAUGHT;
  return true;
}

void cx_impl_throw( char const *file, int line, int xid ) {
  cx_exception = (cx_exception_t){
    .file = file,
    .line = line,
    .thrown_xid = xid
  };
  cx_do_throw();
}

bool cx_impl_try_condition( cx_impl_try_block_t *tb ) {
  assert( tb != NULL );

  switch ( tb->state ) {
    case CX_IMPL_INIT:
      tb->parent = cx_try_block_head;
      cx_try_block_head = tb;
      tb->state = CX_IMPL_TRY;
      return true;
    case CX_IMPL_CAUGHT:
      tb->thrown_xid = 0;               // reset for CX_IMPL_FINALLY case
      FALLTHROUGH;
    case CX_IMPL_TRY:
    case CX_IMPL_THROWN:
      assert( cx_try_block_head != NULL );
      cx_try_block_head = cx_try_block_head->parent;
      tb->state = CX_IMPL_FINALLY;
      return true;
    case CX_IMPL_FINALLY:
      if ( tb->thrown_xid != 0 )
        cx_do_throw();                  // rethrow uncaught exception
      return false;
  } // switch
}

cx_impl_try_block_t cx_impl_try_init( void ) {
  static cx_impl_try_block_t const tb;
  return tb;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
