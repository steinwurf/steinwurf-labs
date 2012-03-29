
Doxygen Style
=============

We use doxygen for all code documentation

* "///" for all normal comments
* "//" for license headers, as they are not comments
* You should also take care that your function arguments are properly documented

  * **@param** for input parameters
  * **@return** for the return value

Below is an example from a documented file

::

   // Some license header
   .
   .
   /// Builds a specific encoder
   /// @param encoder_id specifies the encoder to build
   /// @return the initialized encoder
   pointer_type build(uint32_t encoder_id)
     {
       assert(encoder_id < m_partitioning.blocks());
         /// Build the encoder
         uint32_t symbols =
                m_partitioning.symbols(encoder_id);
       .
       .
     }
   .
   .


Practical Stuff
===============

If you want to generate the doxygen documentation locally, simply use the Doxyfile provided in the repository.

::

   cd kodo/doxygen
   doxygen Doxyfile


If you happen to have some files documentation using "//" instead of "///" you can fix it with the following command.

::

   find . -name "*.h" -print | xargs sed -i 's_ // _ /// _g' 
