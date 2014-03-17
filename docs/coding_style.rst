======================
Steinwurf Coding Style
======================
------
Python
------
For Python the `PEP8 <http://legacy.python.org/dev/peps/pep-0008/>`_ standard
is used. There are plenty of plugins and tools for automatically locate and fix
any diversions from this style.

---
C++
---

Naming
------

* Classes, members, functions, namespaces all lowercase letters separated with
  ``_`` if it enhances readability.
* Template parameters should be CamelCase
* Member variables are prefixed with ``m_``
* In general we try to avoid abbreviations in parameter names, member variables, class names, function names.
* For temporary local variables, you can use abbreviations, single character names as
  long as you use common sense (what you think makes the code most readable).



Include Guards
--------------

Using #pragma once is preferred instead of the lengthy include guards, as this approach is shorter and less error-prone. Furthermore, it might speed up the compilation on modern compilers.

Start every header file like this (after the copyright comment):
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
- Use ``//`` for simple inline C++ comments that are not meant for Doxygen, but for other devs.
- Use ``///`` for comments that are meant for Doxygen (do not use this in function bodies).
- Start comments on new lines if possible

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

      /// Some comment
      void make_me_a_cup()
      {
          // Function body
      }

      /// Another comment
      void better_make_that_two()
      {
          // Function body
      }

  private:

      /// Important functionality
      void grind_beans()
      {
          // Function body
      }
  };

Files and Classes
-----------------
We have a one class per one file rule. If you make a new class ``happy``, then
put it in ``happy.hpp``. This makes the classes easier to find in the
source tree. Exceptions to this rule are nested classes.

Indentation
-----------
We always indent code using SPACES and NOT TABS. The size of an indentation
is 4 spaces.

Warnings
--------
All code should compile without any warnings. Please make sure this is the case on all supported platforms (Linux, Windows, etc.)

Line width
----------
We use the column 80 rule. Break any lines you have that exceed 80 characters.

Testing
-------
Testing is hard, but we try to have a test for all new functionality added in our
projects. For this purpose we use the GoogleTest framework (gtest). You can
find more information on it here: http://code.google.com/p/googletest/

Writing tests
.............
When writing tests remember to:

1. Remove your debug prints before merging with the master.
2. Describe what is the purpose of a test and comment your tests

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

1. In very simple statements (e.g. an if with single statement) you may optionally omit the braces:
   ::
     // Fine
     if (coffee_pot == full)
         continue;

     // Also fine
     if (coffee_pot == empty)
     {
         continue;
     }

2. However in more complicated statements we always put braces - and always with a new-line.
   ::
     // CORRECT (Allman/ANSI-style)
     if (ok == true)
     {
         call_mom();
         call_function();
     }

     // WRONG (in multi-line statements, put the braces)
     if (ok == false)
     {
         // do something fun
     }
     else
         continue;

     // CORRECT
     if (ok == false)
     {
         // do something fun
     }
     else
     {
         continue;
     }

     // WRONG (K&R style)
     if (ok == true) {
         call_function();
     }

Operators
---------
Do not start lines with operators (e.g. +-\*/%&^|:=). Unary operators (e.g. ~-&) are exceptions to this rule.

Add one space around common arithmetic operators to clearly separate the operands.
::
  // CORRECT
  boost::shared_ptr<very_long_type> instance =
      boost::make_shared<very_long_type>(param);

  // WRONG (misplaced '=' sign)
  boost::shared_ptr<very_long_type> instance
      = boost::make_shared<very_long_type>(param);

  // CORRECT
  m_pep = m_pep * std::pow(base, losses + 1.0) +
          (1.0 - std::pow(base, losses));

  // WRONG (misplaced '+' sign)
  m_pep = m_pep * std::pow(base, losses + 1.0)
          + (1.0 - std::pow(base, losses));

  // WRONG (missing spaces)
  m_pep=m_pep*std::pow(base,losses+1.0)+
        (1.0-std::pow(base,losses));

Padding
-------
Padding can greatly improve the readability of long code lines. Always try to keep symmetry and indent continuation lines so that the code is aligned with the counterpart symbols in the previous line. For example:
::
  // Long method signature
  void fake_loopback::send(const uint8_t* data, uint32_t size,
                           const address& address, uint16_t port,
                           fake_udp_socket* socket)

  // Member initializer list
  mutable_storage() :
      m_data(0),
      m_size(0)
  {
      // Constructor body
  }

  // Stack of mixin layers
  template<class Field>
  class on_the_fly_encoder : public
      // Payload Codec API
      payload_encoder<
      // Codec Header API
      systematic_encoder<
      symbol_id_encoder<
      // Symbol ID API
      plain_symbol_id_writer<
      // Coefficient Generator API
      storage_aware_generator<
      uniform_generator<
      // Codec API
      encode_symbol_tracker<
      zero_symbol_encoder<
      linear_block_encoder<
      storage_aware_encoder<
      // Coefficient Storage API
      coefficient_info<
      // Symbol Storage API
      deep_symbol_storage<
      storage_bytes_used<
      storage_block_info<
      // Finite Field API
      finite_field_math<typename fifi::default_field<Field>::type,
      finite_field_info<Field,
      // Factory API
      final_coder_factory_pool<
      // Final type
      on_the_fly_encoder<Field>
      > > > > > > > > > > > > > > > > >
  { };


Declaring pointers and references
---------------------------------

The * and & characters should be part of the type names, and not the variable names.
::
  // CORRECT (C++-style)
  int* pValue;

  // WRONG (C-style)
  int *pValue;

  // CORRECT (C++-style)
  void add(const complex& x, const complex& y)
  {
  }

  // WRONG (C-style)
  void add(const complex &x, const complex &y)
  {
  }

The following regular expressions are helpful to check & replace any violations for this rule:
::
  Find &: ([\w>])\s+&(\w)
  Replace with: $1& $2
  Find *: ([\w>])\s+\*(\w)
  Replace with: $1* $2
  Watch out for return statements like: return *io_ptr;
  Regex to find trailing whitespace: [ \t]+(?=\r?$)


Using asserts
-------------
Using ``asserts`` is a hot-potato in many development discussions. In particiular when talking
about high performance code. In our projects we will adopt the following simple strategy:

* Before **using** a variable or parameter we use an ``assert``:
::

  void test(int a, int* p)
  {
      // We just use the p variable so we only assert on that one. The
      // variable a is only forwarded so it should have an assert elsewhere.
      assert(p);

      p += 10;
      test2(a, p);
  }

Read the following article for more information on this http://queue.acm.org/detail.cfm?id=2220317


Handling Unused Parameters
--------------------------
Use the following approach to handle warnings caused by unused parameters:
::
  void test(int a);
  {
      (void) a;
  }


Hiding Internal Implementation Details
--------------------------------------
To prevent polluting the namespace of a project with internal helper function,
use a nested namespace called detail to hide them:
::
  namespace project_name
  {
      namespace detail
      {
          void help()
          {
              // Do help
          }
      }
      void api()
      {
          // Get help
          detail::help();
      }
  }

An example of this can be seen here https://github.com/steinwurf/sak/blob/8a75568b80c063331ae08d5667a1d67bb92c87b8/src/sak/easy_bind.hpp#L38
