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
