Steinwurf Coding Style
======================

Naming
------

* Classes, members, functions, namespaces all lowercase letters with
  ``_`` if it enhances readability.

* Template parameters CamelCase

* Member variables are prefixed ``_m``



Include Guards
--------------
We do include guards like this:
::
  {PROJECT_NAME}_{INCLUDE_DIRECTORY}_{FILENAME}

So in the case of a file in ``kodo/rlnc/full_vector_codes.h`` it
should be:
::
  KODO_RLNC_FULL_VECTOR_CODES_H

Class Declrarations
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
put it in ``heppy.h``. This makes the classes easier to find in the
source thee. Exceptions to this rule are nested classes.

Indentation
-----------
We always indent code using spaces and not tabs. The size of an indentation is four spaces.
