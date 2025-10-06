# C Exception

## Introduction

**C Exception**
is a library that implements C++-like exceptions in C
using syntax as close as possible to C++.
In addition to `try`, `catch`, and `throw`,
it also offers `finally`
since C lacks destructors.
While the library works,
it has stringent requirements
and several
restrictions.
See the developer documentation for details.

See also [setjmp(), longjmp(), and Exception Handling in C](https://dev.to/pauljlucas/setjmp-longjmp-and-exception-handling-in-c-1h7h).

## Installation

The git repository contains only the necessary source code.
Things like `configure` are _derived_ sources and
[should not be included in repositories](http://stackoverflow.com/a/18732931).
If you have
[`autoconf`](https://www.gnu.org/software/autoconf/),
[`automake`](https://www.gnu.org/software/automake/),
and
[`m4`](https://www.gnu.org/software/m4/)
installed,
you can generate `configure` yourself by doing:

    ./bootstrap

Then follow the generic installation instructions
given in `INSTALL`.

If you would like to generate the developer documentation,
you will also need
[Doxygen](http://www.doxygen.org/);
then do:

    make doc                            # or: make docs

## Building with the Library

When compiling your own code
that uses the library,
you should compile with the following options:

    -Wno-dangling-else -Wno-shadow

(or equivalent for your compiler)
to suppress warnings.

**Paul J. Lucas**  
San Francisco Bay Area, California, USA  
13 October 2023
