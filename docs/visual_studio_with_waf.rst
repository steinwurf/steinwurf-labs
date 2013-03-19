Visual Studio project generator for WAF
=======================================================
This document describes the steps needed to build and run a C/C++ application with WAF 
from Visual Studio as a custom Makefile project.

Disclaimer
----------
These instructions have been tested for the following IDEs:

- Visual Studio 2008
- Visual Studio 2010 C++ Express
- Visual Studio 2012 Express

Note that the only supported compiler is msvc11 (Visual Studio 2012),
but you may use a previous version as an IDE (WAF will invoke the new compiler).

Instructions 
---------------------
1. Run the config helper script in the root of the repository (just double-click on config.py)
2. Choose your makespec and Visual Studio version
3. Open the generated .sln file
4. Build the solution (Ctrl-B or F7)
5. Start without Debugging (Ctrl-F5) to run all the gtests
