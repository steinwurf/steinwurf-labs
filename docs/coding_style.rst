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
We do include guards like this:
::
  {PROJECT_NAME}_{INCLUDE_DIRECTORY}_{FILENAME}

So in the case of a file in ``kodo/rlnc/full_vector_codes.hpp`` it
should be:
::
  KODO_RLNC_FULL_VECTOR_CODES_HPP

Includes (both hpp and cpp)
---------------------------

The most general includes at the top 
::
  #include <vector>
  #include <math>

  #include <boost/shared_ptr>

  #include <fifi/log_table.hpp>
  #include <fifi/is_binary.hpp>

  #include <kodo/storage.hpp>
  
  #include "myfile.hpp"


Header files
------------
We have decided to start using ``.hpp`` for header files. This makes it easier to differentiate between C and C++ code.

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

      // Public members

  private:

      // Private members

  };

With one newline between scope specifiers, members and functions:
::
  class coffee_machine
  {
  public:

      /// Some comment
      void make_me_a_cup();

      /// Another comment
      void better_make_that_two();

  private:

      /// Important functionality
      void grind_beans();

  };

Files and Classes
-----------------

We have a one class one file rule. So if you make a new class ``heppy``
put it in ``heppy.hpp``. This makes the classes easier to find in the
source thee. Exceptions to this rule are nested classes.

Indentation
-----------
We always indent code using spaces and not tabs. The size of an indentation 
is four spaces.

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

1. Remove you debug printing before pushing your changes. 
2. Describe what is the purpose of a tests and comment your tests

Casts
-----

1. Numeric types: If you are casting from a numeric type use either 
   C-style cast or C++ style casts. E.g. both of these are OK:
   ::
     uint32_t o = (uint32_t) some_value;
     uint32_t k = static_cast<uint32_t>(some_value);

   See this http://stackoverflow.com/a/12321860 for more info.

2. All other caes (pointers etc.): Cast using C++ style casts e.g. `static_cast` etc. 


