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
# define CX_THREAD_LOCAL          thread_local
#elif __STDC_VERSION__ >= 201112L
# define CX_THREAD_LOCAL          _Thread_local
#elif defined( _MSC_VER )
# define CX_THREAD_LOCAL          __declspec( thread )
#elif defined( __GNUC__ )
# define CX_THREAD_LOCAL          __thread
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
static CX_THREAD_LOCAL cx_exception_t cx_exception;

/**
 * Current terminate handler.
 */
static cx_terminate_handler_t cx_terminate_handler =
  &cx_default_terminate_handler;

/**
 * Linked list of open `try` blocks.
 */
static CX_THREAD_LOCAL cx_impl_try_block_t *cx_try_block_head;

static cx_xid_matcher_t cx_xid_matcher = &cx_default_xid_matcher;

////////// local functions ////////////////////////////////////////////////////

/**
 * Default terminate handler.
 */
_Noreturn
static void cx_default_terminate_handler( cx_exception_t const *cex ) {
  assert( cex != NULL );
  fprintf( stderr,
    "%s:%d: unhandled exception %d (0x%X)\n",
    cex->file, cex->line,
    cex->xid, (unsigned)cex->xid
  );
  abort();
}

/**
 * Default TODO
 */
static bool cx_default_xid_matcher( int xid1, int xid2 ) {
  return xid1 == xid2;
}

/**
 * TODO
 */
_Noreturn
static void cx_do_throw( void ) {
  if ( cx_try_block_head == NULL )
    cx_terminate();
  cx_try_block_head->state = CX_THROWN;
  cx_try_block_head->xid = cx_exception.xid;
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

bool cx_impl_catch( int xid, cx_impl_try_block_t *tb ) {
  assert( tb != NULL );
  if ( tb->state == CX_FINALLY )
    return false;
  assert( tb->state == CX_THROWN );
  assert( cx_xid_matcher != NULL );
  if ( !(*cx_xid_matcher)( xid, tb->xid ) )
    return false;
  tb->state = CX_CAUGHT;
  return true;
}

bool cx_impl_catch_all( cx_impl_try_block_t *tb ) {
  assert( tb != NULL );
  if ( tb->state == CX_FINALLY )
    return false;
  assert( tb->state == CX_THROWN );
  tb->state = CX_CAUGHT;
  return true;
}

void cx_impl_throw( char const *file, int line, int xid ) {
  cx_exception = (cx_exception_t){ .file = file, .line = line, .xid = xid };
  cx_do_throw();
}

bool cx_impl_try_condition( cx_impl_try_block_t *tb ) {
  assert( tb != NULL );

  switch ( tb->state ) {
    case CX_INIT:
      tb->parent = cx_try_block_head;
      cx_try_block_head = tb;
      tb->state = CX_TRY;
      return true;
    case CX_CAUGHT:
      tb->xid = 0;                      // reset for CX_FINALLY case
      FALLTHROUGH;
    case CX_TRY:
    case CX_THROWN:
      assert( cx_try_block_head != NULL );
      cx_try_block_head = cx_try_block_head->parent;
      tb->state = CX_FINALLY;
      return true;
    case CX_FINALLY:
      if ( tb->xid != 0 )
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
