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

/**
 * @file
 * Declares types, macros, and functions to implement C++-like exception
 * handling in C.
 */

// standard
#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
// While this C library would never be used in a pure C++ program, it may be
// used in a program with both C and C++ code.
extern "C" {
#endif /* __cplusplus */

/// @cond DOXYGEN_IGNORE

#ifndef _Noreturn
# ifdef __cplusplus
#   if __cplusplus >= 201103L
#     define _Noreturn                [[noreturn]]
#   else
#     define _Noreturn                /* nothing */
#   endif
# else /* must be C */
#   if __STDC_VERSION__ >= 202311L
#     define _Noreturn                [[noreturn]]
#   else
#     /* _Noreturn is fine as-is. */
#   endif
# endif /* __cplusplus */
#endif /* _Noreturn */

/// @endcond

////////// public /////////////////////////////////////////////////////////////

/**
 * @defgroup c-exception-public-api-group Public API
 * Declares types, macros, and functions for public use.
 * @{
 */

#if !defined(CX_USE_TRADITIONAL_KEYWORDS)
  /**
   * If defined to 1, allows use of traditional keywords in addition to the
   * default.
   *
   * Default         | Traditional
   * ----------------|------------
   * \ref cx_try     | `try`
   * \ref cx_catch   | `catch`
   * \ref cx_throw   | `throw`
   * \ref cx_finally | `finally`
   *
   * Defaults to 0 to avoid possible collisons with other identifiers.
   */
# define CX_USE_TRADITIONAL_KEYWORDS    0
#endif /* CX_USE_TRADITIONAL_KEYWORDS */

#if !defined(__cplusplus) && CX_USE_TRADITIONAL_KEYWORDS
# define try                      cx_try
# define catch(...)               cx_catch( __VA_ARGS__ )
# define finally                  cx_finally
# define throw(...)               cx_throw( __VA_ARGS__ )
#endif /* !defined(__cplusplus) && CX_USE_TRADITIONAL_KEYWORDS */

/**
 * Matches any exception ID.
 */
#define CX_XID_ANY                0

/**
 * Contains information about a thrown exception.
 */
struct cx_exception {
  /// The file whence the exception was thrown.
  char const *thrown_file;

  /// The line number within \ref thrown_file whence the exception was thrown.
  int         thrown_line;

  /// The exception ID that was thrown.
  int         thrown_xid;

  /// Optional user-data passed via #cx_throw.
  void       *user_data;
};
typedef struct cx_exception cx_exception_t;

/**
 * The signature for a "terminate handler" function that is called by
 * cx_terminate().
 *
 * @warning Terminate handler functions _must not_ return.
 *
 * @param cex A pointer to a cx_exception object that has information about the
 * exception that was thrown.
 *
 * @sa cx_set_terminate()
 */
typedef void (*cx_terminate_handler_t)( cx_exception_t const *cex );

/**
 * The signature for a "exception ID matcher" function that is called by
 * #cx_catch clauses to determine whether the thrown exception matches a
 * particular exception.
 *
 * @remarks
 * @parblock
 * Since C doesn't have inheritance, there's no way to create exception
 * hierarchies.  As a substitute, you can create numeric groups and catch _any_
 * exception in a group.
 *
 * For example, if you have:
 *  ```c
 *  #define EX_FILE_ANY         0x0100
 *  #define EX_FILE_IO_ERROR    (EX_FILE_ANY | 0x01)
 *  #define EX_FILE_NOT_FOUND   (EX_FILE_ANY | 0x02)
 *  #define EX_FILE_PERMISSION  (EX_FILE_ANY | 0x03)
 *  // ...
 *
 *  bool my_cx_xid_matcher( int thrown_xid, int catch_xid ) {
 *    if ( (catch_xid & 0x00FF) == 0x00 )
 *      thrown_xid &= 0xFF00;
 *    return thrown_xid == catch_xid;
 *  }
 *  ```
 * then you can do:
 *  ```c
 *  cx_set_xid_matcher( &my_cx_xid_matcher );
 *  try {
 *    // ...
 *  }
 *  catch( EX_FILE_NOT_FOUND ) {
 *    // handle file-not-found specifically
 *  }
 *  catch( EX_FILE_ANY ) {
 *    // handle any other file error
 *  }
 *  ```
 * @endparblock
 *
 * @param thrown_xid The thrown exception ID.
 * @param catch_xid The exception ID to match \a thrown_xid against.
 * @return Returns `true` only if \a thrown_xid matches \a catch_xid.
 *
 * @sa cx_set_xid_matcher()
 */
typedef bool (*cx_xid_matcher_t)( int thrown_xid, int catch_xid );

/**
 * Begins a "try" block to be followed by zero or more #cx_catch blocks and
 * zero or one #cx_finally block.
 *
 * @warning Any variables declared outside the "try" block that are modified
 * inside the block and used again outside the block _must_ be declared
 * `volatile`:
 *  ```c
 *  int volatile n = 0;
 *  try {
 *    // ..
 *    ++n;
 *  }
 *  catch( EX_FILE_NOT_FOUND ) {
 *    // ...
 *  }
 *  printf( "n = %s\n", n );
 *  ```
 *
 * @warning Within a function that uses a "try" block, you must _never_ use
 * variable-length arrays.
 *
 * @warning Within a "try" block, you must _never_ `break` unless it's within
 * your own loop or `switch` due to the way in which <code>%cx_try</code> is
 * implemented.  For example, do _not_ do something like:
 *  ```c
 *  while ( true ) {
 *    try {
 *      // ...
 *      if ( some_condition )
 *        break;                        // does NOT break out of while loop
 *    }
 *    // ...
 *  }
 *  ```
 * If possible, put the `while` inside the "try" instead:
 *  ```c
 *  try {
 *    while ( true ) {
 *      // ...
 *      if ( some_condition )
 *        break;                        // breaks out of while loop
 *    }
 *    // ...
 *  }
 *  ```
 *
 * @warning Within a "try" block, you must _never_ `goto` outside the block nor
 * `return` from the function. See #cx_cancel_try().
 *
 * @warning Within a "try" block, `continue` will cause the block to exit
 * immediately and jump to the #cx_finally block, if any.
 *
 * @sa #cx_cancel_try()
 * @sa #cx_catch()
 * @sa #cx_finally
 * @sa #cx_throw()
 */
#define cx_try                                                              \
  for ( cx_impl_try_block_t cx_tb = cx_impl_try_init( __FILE__, __LINE__ ); \
        cx_impl_try_condition( &cx_tb ); )                                  \
    if ( cx_tb.state != CX_IMPL_FINALLY )                                   \
      if ( setjmp( cx_tb.env ) == 0 )

/**
 * Begins a "catch" block possibly catching an exception and executing the code
 * in the block.
 *
 * @remarks
 * @parblock
 * This can be used in one of two ways:
 *
 *  1. With an exception ID:
 *      @code
 *      cx_catch( EX_FILE_NOT_FOUND ) {
 *      @endcode
 *     that catches the given exception ID.
 *
 *  2. With no exception ID:
 *      @code
 *      cx_catch() {
 *      @endcode
 *     that catches any exception like the C++ `...` does.
 * @endparblock
 *
 * @note Unlike the C++ equivalent, the `()` are _required_ with _no_ space
 * between the <code>%cx_catch</code> and the `(`.
 *
 * @note For a given #cx_try block, there may be zero or more "catch" blocks.
 * However, if there are zero, there _must_ be one #cx_finally block.  Multiple
 * "catch" blocks are tried in the order declared and at most one "catch" block
 * will be matched.
 *
 * @warning Similarly to a #cx_try block, within a "catch" block, you must
 * _never_ `break` unless it's within your own loop or `switch` due to the way
 * in which <code>%cx_catch</code> is implemented.
 *
 * @warning Within a "catch" block, you must _never_ `goto` outside the block
 * nor `return` from the function. See #cx_cancel_try().
 *
 * @warning Within a "catch" block, `continue` will cause the block to exit
 * immediately and jump to the #cx_finally block, if any.
 *
 * @sa #cx_cancel_try()
 * @sa #cx_finally
 * @sa #cx_set_xid_matcher()
 * @sa #cx_throw()
 * @sa #cx_try
 */
#define cx_catch(...) \
  CX_IMPL_NAME2(CX_IMPL_CATCH_, CX_IMPL_NARG(__VA_ARGS__))(__VA_ARGS__)

/**
 * Begins a "finally" block always executing the code in the block after the
 * code in the #cx_try block and any #cx_catch block.
 *
 * @remarks Even though C++ doesn't have `finally`, it's provided since C
 * doesn't have destructors to implement
 * [RAII](https://en.cppreference.com/w/cpp/language/raii).
 *
 * @note For a given #cx_try block, there may be zero or one "finally" block.
 * However, if there are zero #cx_catch blocks, then there _must_ be one
 * "finally" block.
 *
 * @warning Similarly to a #cx_try block, within a "finally" block, you must
 * _never_ `break` unless it's within your own loop or `switch` due to the way
 * in which <code>%cx_finally</code> is implemented.
 *
 * @warning Within a "finally" block, you must _never_ `goto` outside the block
 * nor `return` from the function. See #cx_cancel_try().
 *
 * @warning Within a "finally" block, `continue` will cause the block to exit
 * immediately.  If there is an uncaught exception, it will be rethrown.
 *
 * @sa #cx_cancel_try()
 * @sa #cx_try
 * @sa #cx_catch
 * @sa #cx_throw
 */
#define cx_finally                                \
      else /* setjmp() != 0 */ /* do nothing */;  \
    else /* cx_tb.state == CX_IMPL_FINALLY */

/**
 * Throws an exception.
 *
 * @remarks
 * @parblock
 * This can be called in one of three ways:
 *
 *  1. With an exception ID:
 *     @code
 *      cx_throw( EX_FILE_NOT_FOUND );
 *     @endcode
 *     that throws a new exception.  It may be any non-zero value.
 *
 *  2. With an exception ID and user-data:
 *     @code
 *      cx_throw( EX_FILE_NOT_FOUND, path );
 *     @endcode
 *     that throws a new exception passing `path` as a `void*` argument.
 *
 *  3. Without no arguments:
 *     @code
 *      cx_throw();
 *     @endcode
 *     that rethrows the most recent exception with the same user-data, if any.
 *     If no exception has been caught, calls cx_terminate().
 * @endparblock
 *
 * @note Unlike C++, the `()` are _required_ with _no_ space between the
 * <code>%cx_throw</code> and the `(`.
 *
 * @warning An exception that is thrown but not caught will result in
 * cx_terminate() being called.
 *
 * @sa #cx_set_terminate()
 * @sa #cx_try
 * @sa #cx_catch()
 * @sa #cx_finally
 */
#define cx_throw(...) \
  CX_IMPL_NAME2(CX_IMPL_THROW_, CX_IMPL_NARG(__VA_ARGS__))(__VA_ARGS__)

/**
 * Cancels a current #cx_try, #cx_catch, or #cx_finally block in the current
 * scope allowing you to then safely `goto` out of the block or `return` from
 * the function:
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
 * @warning After calling <code>%cx_cancel_try()</code>, you _must_ exit the
 * block via `break`, `goto`, or `return`.
 *
 * @warning Calling <code>%cx_cancel_try()</code> from a #cx_try or #cx_catch
 * block will result in the #cx_finally block, if any, _not_ being executed.
 *
 * @warning Any uncaught exception will _not_ be rethrown.
 */
#define cx_cancel_try()           cx_impl_cancel_try( &cx_tb )

/**
 * Gets the current exception, if any.
 *
 * @return If an exception is in progress, returns a pointer to it; otherwise
 * returns NULL.
 *
 * @sa cx_user_data()
 */
cx_exception_t* cx_current_exception( void );

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
 * Gets the current \ref cx_xid_matcher_t, if any.
 *
 * @return Returns said function or NULL if none.
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
 * Sets teh current \ref cx_xid_matcher_t.
 *
 * @param fn The new \ref cx_xid_matcher_t or NULL to use the default.
 * @return Returns the previous \ref cx_xid_matcher_t, if any.
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

/**
 * Gets the user-data, if any, associated with the current exception, if any.
 *
 * @return If an exception is in progress, returns the user-data; otherwise
 * returns NULL.
 *
 * @sa cx_current_exception()
 */
inline void* cx_user_data( void ) {
  cx_exception_t *const cex = cx_current_exception();
  return cex != NULL ? cex->user_data : NULL;
}

/** @} */

////////// implementation /////////////////////////////////////////////////////

/**
 * @defgroup c-exception-implementation-group Implementation API
 * Declares types, macros, and functions for the implementation.
 *
 * @note Everything in the implementation API starts with either `cx_impl_` or
 * `CX_IMPL_`.
 *
 * @{
 */

/// @cond DOXYGEN_IGNORE

// See <https://stackoverflow.com/a/11742317/99089>

#define CX_IMPL_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) N
#define CX_IMPL_COMMA(...)        ,
#define CX_IMPL_COMMA_SEQ_N       1, 1, 1, 1, 1, 1, 1, 1, 0, 0
#define CX_IMPL_NARG_(...)        CX_IMPL_ARG_N( __VA_ARGS__ )
#define CX_IMPL_REV_SEQ_N         9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define CX_IMPL_HAS_COMMA(...) \
  CX_IMPL_NARG_( __VA_ARGS__, CX_IMPL_COMMA_SEQ_N )

#define CX_IMPL_NARG(...)                               \
  CX_IMPL_NARG_HELPER1(                                 \
    CX_IMPL_HAS_COMMA( __VA_ARGS__ ),                   \
    CX_IMPL_HAS_COMMA( CX_IMPL_COMMA __VA_ARGS__ () ),  \
    CX_IMPL_NARG_( __VA_ARGS__, CX_IMPL_REV_SEQ_N ) )

#define CX_IMPL_NARG_HELPER1(A,B,N)   CX_IMPL_NARG_HELPER2(A, B, N)
#define CX_IMPL_NARG_HELPER2(A,B,N)   CX_IMPL_NARG_HELPER3_ ## A ## B(N)
#define CX_IMPL_NARG_HELPER3_01(N)    0
#define CX_IMPL_NARG_HELPER3_00(N)    1
#define CX_IMPL_NARG_HELPER3_11(N)    N

#define CX_IMPL_NAME2(A,B)        CX_IMPL_NAME2_HELPER(A,B)
#define CX_IMPL_NAME2_HELPER(A,B) A##B

#define CX_IMPL_CATCH_0()         CX_IMPL_CATCH_1( CX_XID_ANY )
#define CX_IMPL_CATCH_1(XID)      else if ( cx_impl_catch( (XID), &cx_tb ) )

#define CX_IMPL_THROW_0()         CX_IMPL_THROW_1( cx_tb.thrown_xid )
#define CX_IMPL_THROW_1(XID)      CX_IMPL_THROW_2( (XID), cx_user_data() )

#define CX_IMPL_THROW_2(XID,DATA) \
  cx_impl_throw( __FILE__, __LINE__, (XID), (void*)(DATA) )

/// @endcond

/**
 * Internal state of by \ref cx_impl_try_block.
 */
enum cx_impl_state {
  CX_IMPL_INIT,                         ///< Initial state.
  CX_IMPL_TRY,                          ///< No exception thrown.
  CX_IMPL_THROWN,                       ///< Exception thrown, but uncaught.
  CX_IMPL_CAUGHT,                       ///< Exception caught.
  CX_IMPL_FINALLY                       ///< Running #cx_finally code, if any.
};
typedef enum cx_impl_state cx_impl_state_t;

typedef struct cx_impl_try_block cx_impl_try_block_t;

/**
 * Internal state of #cx_try block.
 */
struct cx_impl_try_block {
  char const           *try_file;       ///< File containing the #cx_try.
  int                   try_line;       ///< Line within \ref try_file.
  jmp_buf               env;            ///< Jump buffer.
  cx_impl_try_block_t  *parent;         ///< Enclosing parent #cx_try, if any.
  cx_impl_state_t       state;          ///< Current state.
  int                   thrown_xid;     ///< Thrown exception ID, if any.
  int                   caught_xid;     ///< Caught exception ID, if any.
#ifndef NDEBUG
  /// Prevents infinite loops.
  unsigned              try_condition_calls;
#endif /* NDEBUG */
};

/**
 * Catches exception \a xid.
 *
 * @param xid The exception ID to catch.  If #CX_XID_ANY, it is always caught.
 * @param tb A pointer to the current \ref cx_impl_try_block.
 * @return Returns `true` only if \a xid was caught.
 */
bool cx_impl_catch( int xid, cx_impl_try_block_t *tb );

/**
 * Implements #cx_cancel_try().
 *
 * @param tb A pointer to the current \ref cx_impl_try_block.
 *
 * @sa #cx_cancel_try()
 */
void cx_impl_cancel_try( cx_impl_try_block_t const *tb );

/**
 * Implements #cx_throw()
 *
 * @param throw_file The file whence the exception wat thrown.
 * @param throw_line The line number within \a throw_file whence the exception
 * was thrown.
 * @param xid The exception ID to throw.  It may be any non-zero value.
 * @param user_data Optional user-data copied into \ref cx_exception::user_data
 * "user_data".
 */
_Noreturn
void cx_impl_throw( char const *throw_file, int throw_line, int xid,
                    void *user_data );

/**
 * Checks whether the #cx_try, #cx_catch, or #cx_finally code should be
 * executed.
 *
 * @param tb A pointer to the current \ref cx_impl_try_block.
 * @return Returns `true` only if the code should be executed.
 */
bool cx_impl_try_condition( cx_impl_try_block_t *tb );

/**
 * Gets an initialized \ref cx_impl_try_block.
 *
 * @param try_file The file containing the #cx_try.
 * @param try_line The line withing \ref file containing the #cx_try.
 * @return Returns an initialized \ref cx_impl_try_block.
 */
cx_impl_try_block_t cx_impl_try_init( char const *try_file, int try_line );

/** @} */

///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* C_EXCEPTION_H */
/* vim:set et sw=2 ts=2: */
