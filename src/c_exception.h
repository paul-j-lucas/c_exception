/*
**      C Exception -- Exception Library for C
**      src/c_exception.h
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

#ifndef C_EXCEPTION_H
#define C_EXCEPTION_H

// standard
#include <setjmp.h>
#include <stdbool.h>

#ifdef CX_USE_CPP_KEYWORDS
# define try                      cx_try
# define catch(...)               cx_catch( __VA_ARGS__ )
# define finally                  cx_finally
# define throw(...)               cx_throw( __VA_ARGS__ )
#endif /* CX_USE_CPP_KEYWORDS */

///////////////////////////////////////////////////////////////////////////////

/**
 * TODO
 */
struct cx_exception {
  void       *user_data;
  char const *file;     ///< The file whence the exception was thrown.
  int         line;     ///< The line number within \ref file whence the exception was thrown.
  int         xid;      ///< The exception ID that was thrown.
};
typedef struct cx_exception cx_exception_t;

/**
 * The signature for a "terminate handler" function that is called by
 * cx_terminate().
 *
 * @note Terminate handler functions _must not_ return.
 */
typedef void (*cx_terminate_handler_t)( cx_exception_t const *cex );

/**
 * TODO
 *
 * @param xid1 The first exception ID.
 * @param xid2 The second exception ID.
 * @return Returns `true` only if \a xid1 matches \a xid2.
 */
typedef bool (*cx_xid_matcher_t)( int xid1, int xid2 );

/**
 * Begins a `try` block to be followed by one or more #cx_catch blocks.
 *
 * @warning Within a `try` block, you must _never_ use `break` or `continue`
 * (unless within your own `switch` or loop, nor `goto` outside the `try`
 * block, nor `return` unless you call cx_cancel_try() first.
 *
 * @warning However, even if you call cx_cancel_try(), either a `break` or
 * `continue` will _not_ break out of a loop surrounding the `try` block due to
 * the way in which <code>%try</code> is implemented:
 *  ```c
 *  while ( true ) {
 *    try {
 *      // ...
 *      if ( do_break ) {
 *        cx_cancel_try();
 *        break;                        // does NOT break out of while loop
 *      }
 *    }
 *    // ...
 *  }
 *  ```
 * If possible, put the `while` inside the `try` instead:
 *  ```c
 *  try {
 *    while ( true ) {
 *      // ...
 *      if ( do_break )
 *        break;                        // breaks out of while loop
 *    }
 *    // ...
 *  }
 *  ```
 *
 * @sa cx_cancel_try()
 * @sa #cx_catch()
 * @sa #cx_finally
 * @sa #cx_throw()
 */
#define cx_try                                          \
  for ( cx_impl_try_block_t cx_tb = cx_impl_try_init(); \
        cx_impl_try_condition( &cx_tb ); )              \
    if ( cx_tb.state == CX_TRY && (cx_tb.xid = setjmp( cx_tb.env )) == 0 )

/**
 * Begins a `catch` block possibly catching an exception and executing the code
 * in the block.
 *
 * @remarks
 * @parblock
 * This can be used in one of two ways:
 *
 *  1. With an exception ID:
 *      ```c
 *      cx_catch( EX_FILE_NOT_FOUND ) {
 *      ```
 *     that catches the given exception ID.
 *
 *  2. With no exception ID:
 *      ```c
 *      cx_catch() {
 *      ```
 *     that catches any exception like the C++ `...` does.
 * @endparblock
 *
 * @note Unlike the C++ equivalent, the `()` are required with _no_ space
 * between the <code>%catch</code> and the `(`.
 *
 * @note Catch blocks are optional.
 *
 * @warning The same warnings about `try` also apply to catch blocks.
 *
 * @sa #cx_throw()
 * @sa #cx_try
 * @sa #cx_finally
 */
#define cx_catch(...) \
  CX_NAME2(CX_CATCH_, CX_ARGN(CX_COMMA __VA_ARGS__ ()))(__VA_ARGS__)

/**
 * Begins a `finally` block always executing the code in the block after the
 * code in the try block and any catch block.
 *
 * @remarks Even though C++ doesn't have `finally`, it's provided since C
 * doesn't have destructors to implement RAII.
 *
 * @note Finally blocks are optional.
 *
 * @warning The same warnings about `try` also apply to finally blocks.
 *
 * @sa #cx_try
 * @sa #cx_catch
 * @sa #cx_throw
 */
#define cx_finally                else if ( cx_tb.state == CX_FINALLY )

/**
 * Throws an exception.
 *
 * @remarks
 * @parblock
 * This can be called in one of two ways:
 *
 *  1. With an exception ID:
 *      ```c
 *      cx_throw( EX_FILE_NOT_FOUND );
 *      ```
 *     that throws a new exception.
 *
 *  2. Without an exception ID:
 *      ```c
 *      cx_throw();
 *      ```
 *     that rethrows the most recent exception.  If no exception has been
 *     caught, calls cx_terminate().
 * @endparblock
 *
 * @note Unlike C++, the `()` are required with _no_ space between the
 * <code>%cx_throw</code> and the `(`.
 *
 * @warning An exception that is thrown but not caught will result in
 * cx_terminate() being called.
 *
 * @sa #cx_try
 * @sa #cx_catch()
 * @sa #cx_finally
 */
#define cx_throw(...) \
  CX_NAME2(CX_THROW_, CX_ARGN(CX_COMMA __VA_ARGS__ ()))(__VA_ARGS__)

/**
 * Cancels a current `try` block in the current scope allowing you to then
 * safely `goto` out of the `try` block or `return` from the function.
 *
 *  ```c
 *  try {
 *    // ...
 *    if ( return_early ) {
 *        cx_cancel_try();
 *        return;
 *    }
 *  }
 *  ```
 *
 * @note If there is no current `try` block, does nothing.
 */
void cx_cancel_try( void );

/**
 * Gets the current \ref cx_terminate_handler_t, if any.
 *
 * @return Returns said handler or NULL if none.
 *
 * @sa cx_set_terminate()
 * @sa cx_terminate()
 */
cx_terminate_handler_t cx_get_terminate( void );

/**
 * TODO
 *
 * @return TODO
 *
 * @sa cx_set_xid_matcher()
 */
cx_xid_matcher_t cx_get_xid_matcher( void );

/**
 * Sets the current \ref cx_terminate_handler_t.
 *
 * @param fn The new \ref cx_terminate_handler_t or NULL to use the default.
 * @return Returns the previous \ref cx_terminate_handler_t, if any.
 *
 * @sa cx_get_terminate()
 * @sa cx_terminate()
 */
cx_terminate_handler_t cx_set_terminate( cx_terminate_handler_t fn );

/**
 * TODO
 *
 * @param fn TODO
 * @return TODO
 *
 * @sa cx_get_xid_matcher()
 */
cx_xid_matcher_t cx_set_xid_matcher( cx_xid_matcher_t fn );

/**
 * Calls the current \ref cx_terminate_handler_t function.
 *
 * @sa cx_get_terminate()
 * @sa cx_set_terminate()
 */
_Noreturn
void cx_terminate( void );

////////// implementation /////////////////////////////////////////////////////

#define CX_ARGN(...)              CX_COUNT(__VA_ARGS__, 2, 0, 1)
#define CX_COMMA(...)             ,
#define CX_COUNT(_1,_2,_3,COUNT,...) COUNT

#define CX_NAME2(A,B)             CX_NAME2_HELPER(A,B)
#define CX_NAME2_HELPER(A,B)      A##B

#define CX_CATCH_0()              else if ( cx_impl_catch_all( &cx_tb ) )
#define CX_CATCH_1(XID)           else if ( cx_impl_catch( (XID), &cx_tb ) )

#define CX_THROW_0()              CX_THROW_1( cx_tb.xid )
#define CX_THROW_1(XID)           cx_impl_throw( __FILE__, __LINE__, (XID) )

/**
 * Internal state maintained by \ref cx_impl_try_block.
 */
enum cx_impl_state {
  CX_INIT,                              ///< Initial state.
  CX_TRY,                               ///< No exception thrown.
  CX_THROWN,                            ///< Exception thrown, but uncaught.
  CX_CAUGHT,                            ///< Exception caught.
  CX_FINALLY                            ///< Running `finally` code, if any.
};
typedef enum cx_impl_state cx_impl_state_t;

typedef struct cx_impl_try_block cx_impl_try_block_t;

/**
 * TODO
 */
struct cx_impl_try_block {
  jmp_buf               env;            ///< Jump buffer.
  cx_impl_try_block_t  *parent;         ///< Enclosing parent `try`, if any.
  cx_impl_state_t       state;          ///< Current state.
  int                   xid;            ///< Thrown exception ID, if any.
};

/**
 * Catches exception \a xid.
 *
 * @param xid The exception ID to catch.
 * @param tb A pointer to the current \ref cx_impl_try_block.
 * @return Returns `true` only if \a xid was caught.
 */
bool cx_impl_catch( int xid, cx_impl_try_block_t *tb );

/**
 * Catches any exception.
 *
 * @param tb A pointer to the current \ref cx_impl_try_block.
 * @return Always returns `true` except when \a tb->state is #CX_FINALLY.
 */
bool cx_impl_catch_all( cx_impl_try_block_t *tb );

/**
 * TODO
 *
 * @param file The file whence the exception wat thrown.
 * @param line The line number within \a file whence the exception was thrown.
 * @param xid The exception ID.
 */
_Noreturn
void cx_impl_throw( char const *file, int line, int xid );

/**
 * TODO
 *
 * @param tb TODO
 * @return TODO
 */
bool cx_impl_try_condition( cx_impl_try_block_t *tb );

/**
 * TODO
 *
 * @return TODO
 */
cx_impl_try_block_t cx_impl_try_init( void );

///////////////////////////////////////////////////////////////////////////////
#endif /* C_EXCEPTION_H */
/* vim:set et sw=2 ts=2: */
