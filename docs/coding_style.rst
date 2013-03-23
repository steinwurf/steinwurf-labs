Steinwurf Coding Style
======================

Naming
------

* Classes, members, functions, namespaces all lowercase letters with
  ``_`` if it enhances readability.

* Template parameters CamelCase

* Member variables are prefixed ``m_``



Include Guards
--------------

Using #pragma once is preferred instead of the lengthy include guards, as this approach is shorter and less error-prone. Furthermore, it might speed up the compilation on modern compilers.

Start every header file like this (after copyright comment):
::
  #pragma once


Includes (both hpp and cpp)
---------------------------

The most general includes should be placed at the top:
::
  #include <vector>
  #include <math>

  #include <boost/shared_ptr>

  #include <fifi/log_table.hpp>
  #include <fifi/is_binary.hpp>

  #include <kodo/storage.hpp>
  
  #include "myfile.hpp"


Header file extension
---------------------

We have decided to start using ``.hpp`` for header files. This makes it easier to differentiate between C and C++ code.

Comments
--------
- Use ``//`` for simple inline c++ comments that are not meant for doxygen, but for other devs.
- Use ``///`` for comments that are meant for doxygen.

Class Declarations
-------------------

We group private and public functions and members in different sections:
::
  class foo
  {
  public:

      // Public functions

  private:

      // Private functions

  public:

      // Public members (avoid these!)

  private:

      // Private members

  };

With one newline between scope specifiers, members and functions:
::
  class coffee_machine
  {
  public:

      // Some comment
      void make_me_a_cup();

      // Another comment
      void better_make_that_two();

  private:

      // Important functionality
      void grind_beans();

  };

Files and Classes
-----------------

We have a one class per one file rule. If you make a new class ``happy``, then
put it in ``happy.hpp``. This makes the classes easier to find in the
source tree. Exceptions to this rule are nested classes.

Indentation
-----------
We always indent code using spaces and not tabs. The size of an indentation 
is 4 spaces.

Warnings
--------
- All code should compile without any warnings. Please 
  make sure this is the case on all supported platforms (Linux, Windows, etc.)

Line-width
----------
- We use the column 80 rule. Break any lines you have that exceed 80 characters.

Testing
-------
Testing is hard, but we try to have a test for all new functionality added in our
projects. For this purpose we use the Google C++ Test Framework (gtest). You can
find more information on it here: http://code.google.com/p/googletest/

Writing tests
.............
When writing tests remember to:

1. Remove your debug prints before pushing your changes. 
2. Describe what is the purpose of a tests and comment your tests

Casts
-----

1. Numeric types: If you are casting from a numeric type use either 
   C-style cast or C++ style casts. E.g. both of these are fine:
   ::
     uint32_t o = (uint32_t) some_value;
     uint32_t k = static_cast<uint32_t>(some_value);

   See this http://stackoverflow.com/a/12321860 for more info.

2. All other cases (pointers etc.): Cast using C++ style casts e.g. `static_cast` etc. 

Braces
------

1. In very simple statements e.g. an if with single statment you may optionally omit the braces:
   ::
     // Is ok
     if(coffee_pot == full)
         continue;

     // Is also ok
     if(coffee_pot == empty)
     { 
         continue;
     }

2. However in more complicated statements we always put braces - and always with a new-line.
   ::
     // Correct (Allman/ANSI-style)
     if(ok == true)
     {
         call_mom();
         call_function();
     }

     // Wrong (in multiline statements, put the braces)
     if(ok == false)
     {
         // do something fun
     }
     else
         continue;

     // Correct 
     if(ok == false)
     {
         // do something fun
     }
     else
     {
         continue;
     }
     
     // Also wrong (K&R style)
     if(ok == true) {
         call_function();
     }

Declaring pointers and references
---------------------------------

We believe that the * and & characters should be part of the type names, and not the variable names. 
::
  // CORRECT (C++-style)
  int* pValue;

  // Wrong (C-style)
  int *pValue;

  // CORRECT (C++-style)
  void add(const complex& x, const complex& y)
  {
  }

  // Wrong (C-style)
  void add(const complex &x, const complex &y)
  {
  }

The following regular expressions are helpful to check&replace any violations for this rule:
::
  Find &: ([\w>])\s+&(\w)
  Replace with: $1& $2
  Find *: ([\w>])\s+\*(\w) 
  Replace with: $1* $2
  Watch out for return statements like: return *io_ptr;
  Regex to find trailing whitespace: [ \t]+(?=\r?$)
